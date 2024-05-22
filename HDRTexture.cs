using Silk.NET.Assimp;
using Silk.NET.OpenGL;
using StbImageSharp;
using System;
using System.Numerics;
using System.Runtime.CompilerServices;
using File = System.IO.File;

namespace HW2
{
    public class HDRTexture : IDisposable
    {
        private readonly uint _handle;
        private readonly GL _gl;
        private readonly ImageResultFloat imageResult;

        public uint Handle => _handle;
        public TextureUnit Unit { get; private set; }

        public int Width { get; }
        public int Height { get; }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public Vector3 Sample(int x, int y)
        {
            var location = new ArraySegment<float>(imageResult.Data, (Width*3*x) + (3*y), 3);
            return new Vector3(location);
        }

        public unsafe HDRTexture(GL gl, string path)
        {
            _gl = gl;
            _handle = _gl.GenTexture();
            Bind();

            using (var stream = File.OpenRead(path))
            {
                imageResult = ImageResultFloat.FromStream(stream, ColorComponents.RedGreenBlue);
                Width = imageResult.Width;
                Height = imageResult.Height;
                gl.TexImage2D<float>(TextureTarget.Texture2D,
                                     0,
                                     InternalFormat.Rgb32f,
                                     (uint)imageResult.Width,
                                     (uint)imageResult.Height,
                                     0,
                                     PixelFormat.Rgb,
                                     PixelType.Float,
                                     imageResult.Data.AsSpan());
            }

            SetParameters();
        }

        private void SetParameters()
        {
            _gl.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapS, (int)GLEnum.ClampToEdge);
            _gl.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapT, (int)GLEnum.ClampToEdge);
            _gl.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMinFilter, (int)GLEnum.LinearMipmapLinear);
            _gl.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMagFilter, (int)GLEnum.Linear);
            _gl.GenerateMipmap(TextureTarget.Texture2D);
        }

        public void Bind(TextureUnit unit = TextureUnit.Texture0)
        {
            _gl.ActiveTexture(unit);
            _gl.BindTexture(TextureTarget.Texture2D, _handle);
            Unit = unit;
        }

        public void Dispose()
        {
            _gl.DeleteTexture(_handle);
        }
    }
}
