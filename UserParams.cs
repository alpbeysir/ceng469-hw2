using Silk.NET.Input;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HW2
{
    public enum VisMode
    {
        LightProbe,
        Mirror,
        Glass,
        Glossy,
        SpecularDisco
    }

    internal static class UserParams
    {
        public static float Exposure { get; private set; }
        public static float SkyboxRotation { get; private set; }
        public static bool SpecularEnabled { get; private set; } = true;
        public static int AlgoN { get; private set; }
        public static VisMode VisMode = VisMode.LightProbe;

        public static void OnUpdate(double delta, IKeyboard keyboard)
        {
            float speed = 20f * (float)delta;
            if (keyboard.IsKeyPressed(Key.A))
            {
                SkyboxRotation += speed;
            }
            if (keyboard.IsKeyPressed(Key.D))
            {
                SkyboxRotation -= speed; 
            }
        }

        public static void ProcessKey(IKeyboard keyboard, Key key, int arg3)
        {
            switch (key)
            {
                case Key.W:
                    Exposure *= 2;
                    break;
                case Key.S:
                    Exposure /= 2;
                    break;
                case Key.F:
                    SpecularEnabled = !SpecularEnabled;
                    break;
                case Key.E:
                    AlgoN = Math.Clamp(AlgoN - 1, 1, 7);
                    break;
                case Key.R:
                    AlgoN = Math.Clamp(AlgoN + 1, 1, 7);
                    break;
                case Key.Number1:
                    VisMode = VisMode.LightProbe;
                    break;
                case Key.Number2:
                    VisMode = VisMode.Mirror;
                    break;
                case Key.Number3:
                    VisMode = VisMode.Glass;
                    break;
                case Key.Number4:
                    VisMode = VisMode.Glossy;
                    break;
                case Key.Number5:
                    VisMode = VisMode.SpecularDisco;
                    break;
                case Key.Escape:
                    Environment.Exit(0);
                    break;
            }
        }
    }
}
