using HW2.Helpers;
using Silk.NET.Assimp;
using Silk.NET.Maths;
using Silk.NET.OpenGL;
using System.Numerics;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace HW2
{
    [StructLayout(LayoutKind.Sequential)]
    internal struct Light(Vector3 position, Vector3 color)
    {
        public Vector3 Position = position;
        public float zort;
        public Vector3 Color = color;
        public float zort2;

        public override readonly string ToString()
        {
            return $"Position: {Position}, Luminance: {Color}";
        }
    }

    internal class MedianCut
    {
        private const int LevelCount = 7;
        public readonly List<Light>[] lightingData = new List<Light>[LevelCount];
        private readonly HDRTexture hdr;
        private readonly Vector3[,] SAT;
        private readonly GL gl;

        private readonly Vector3 RGBConst = new(0.2126f, 0.7152f, 0.0722f);

        private uint ubo;

        internal void SetupLightUBO()
        {
            ubo = gl.GenBuffer();
            gl.BindBuffer(BufferTargetARB.UniformBuffer, ubo);
            gl.BufferData(BufferTargetARB.UniformBuffer, (nuint)(128 * Marshal.SizeOf<Light>()), in IntPtr.Zero, BufferUsageARB.DynamicDraw);
            gl.BindBufferBase(BufferTargetARB.UniformBuffer, 0, ubo);
        }

        internal void UploadLightUBO(int level)
        {
            gl.BindBuffer(BufferTargetARB.UniformBuffer, ubo);
            gl.BufferData(GLEnum.UniformBuffer,
                        (nuint)(lightingData[level].Count * Unsafe.SizeOf<Light>()),
                        [.. lightingData[level]],
                        BufferUsageARB.DynamicDraw);
        }

        private Vector3 SampleWeighted(int x, int y)
        {
            return Inclination(y) * hdr.Sample(y, x);
        }

        private float Luminance(Vector3 color)
        {
            return Vector3.Dot(color, RGBConst);
        }

        private Vector3 GetAreaSum(Vector2D<int> TL, Vector2D<int> BR)
        {
            Vector3 sum = SAT[BR.X, BR.Y];
            if (TL.X > 0)
                sum -= SAT[TL.X - 1, BR.Y];
            if (TL.Y > 0)
                sum -= SAT[BR.X, TL.Y - 1];
            if (TL.X > 0 && TL.Y > 0)
                sum += SAT[TL.X - 1, TL.Y - 1];
            return sum;
        }

        private Vector3 GetLightWorldPosition(Vector2 screenPos)
        {
            float u = screenPos.X / hdr.Width;
            float v = -screenPos.Y / hdr.Height;

            float theta = u * 2.0f * MathF.PI;
            float phi = v * MathF.PI;

            float x = MathF.Sin(phi) * MathF.Cos(theta);
            float y = MathF.Cos(phi);
            float z = MathF.Sin(phi) * MathF.Sin(theta);

            return new Vector3(x, y, z);
        }

        internal MedianCut(GL gl, HDRTexture hdr)
        {
            this.hdr = hdr;
            this.gl = gl;
            SetupLightUBO();

            for (int i = 0; i < LevelCount; i++)
            {
                lightingData[i] = [];
            }

            SAT = new Vector3[hdr.Width, hdr.Height];

            SAT[0, 0] = SampleWeighted(0, 0);

            for (int i = 1; i < hdr.Width; i++)
            {
                SAT[i, 0] = SampleWeighted(i, 0) + SAT[i - 1, 0];
            }

            for (int j = 1; j < hdr.Height; j++)
            {
                SAT[0, j] = SampleWeighted(0, j) + SAT[0, j - 1];
            }

            for (int i = 1; i < hdr.Width; i++)
            {
                for (int j = 1; j < hdr.Height; j++)
                {
                    SAT[i, j] = SAT[i, j - 1] + SAT[i - 1, j] - SAT[i - 1, j - 1] + SampleWeighted(i, j);
                }
            }

            Calculate(new(0, 0), new(hdr.Width - 1, hdr.Height - 1), 0);
            //float total = Luminance(GetAreaSum(new(0, 0), new(hdr.Width - 1, hdr.Height - 1)));
            //float test = lightingData[6].Sum(l => Luminance(l.Color));
        }

        static float Map(float x, float in_min, float in_max, float out_min, float out_max)
        {
            return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
        }

        private float Inclination(float y)
        {
            float ratio = y / hdr.Height;
            float deg = Map(ratio, 0.0f, 1.0f, -90.0f, 90.0f);
            return MathF.Cos(MathHelper.DegreesToRadians(deg));
        }

        private void Calculate(Vector2D<int> TL, Vector2D<int> BR, int level)
        {
            if (level >= LevelCount) return;

            Vector2D<int> leftRegionBR = new(TL.X, BR.Y);
            Vector2D<int> rightRegionTL = new(TL.X + 1, TL.Y);
            while (leftRegionBR.X < BR.X)
            {
                float leftSum = Luminance(GetAreaSum(TL, leftRegionBR));
                float rightSum = Luminance(GetAreaSum(rightRegionTL, BR));
                if (leftSum >= rightSum)
                    break;

                leftRegionBR.X++;
                rightRegionTL.X++;
            }

            Vector2D<int> topRegionBR = new(BR.X, TL.Y + 1);
            Vector2D<int> bottomRegionTL = new(TL.X, TL.Y + 1);
            while (topRegionBR.Y < BR.Y)
            {
                float topSum = Luminance(GetAreaSum(TL, topRegionBR));
                float bottomSum = Luminance(GetAreaSum(bottomRegionTL, BR));
                if (topSum >= bottomSum)
                    break;

                topRegionBR.Y++;
                bottomRegionTL.Y++;
            }

            Vector2 centroid = new((leftRegionBR.X + rightRegionTL.X) / 2.0f, (topRegionBR.Y + bottomRegionTL.Y) / 2.0f);
            Light light = new(GetLightWorldPosition(centroid), GetAreaSum(TL, BR));
            lightingData[level].Add(light);

            int width = BR.X - TL.X;
            int height = BR.Y - TL.Y;

            if (width > height)
            {
                Calculate(TL, leftRegionBR, level + 1);
                Calculate(rightRegionTL, BR, level + 1);
            }
            else
            {
                Calculate(TL, topRegionBR, level + 1);
                Calculate(bottomRegionTL, BR, level + 1);
            }
        }
    }
}
