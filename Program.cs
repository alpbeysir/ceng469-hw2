using Silk.NET.Input;
using Silk.NET.OpenGL;
using Silk.NET.Windowing;
using System;
using System.Linq;
using System.Numerics;
using Silk.NET.Maths;
using Silk.NET.Input.Glfw;
using Silk.NET.Windowing.Glfw;
using HW2.Helpers;
using static Silk.NET.Core.Native.WinString;

namespace HW2
{
    class Program
    {
        const uint sideWidth = 2048;
        const uint sideHeight = 2048;

        private static IWindow window;
        private static GL Gl;

        private static HDRTexture HDRI;
        private static CubemapTexture CubemapTexture;

        private static Mesh Mesh;
        private static Mesh DebugMesh;
        private static Skybox Skybox;
        private static MedianCut MedianCut;
        private static Vector3 CameraGaze;

        //Used to track change in mouse movement to allow for moving of the Camera
        private static Vector2 LastMousePosition;
        private static float CameraYaw = 45.0f;
        private static float CameraPitch = 45.0f;
        private static IKeyboard Keyboard;

        private static Helpers.Shader MirrorShader;
        private static Helpers.Shader GlassShader;
        private static Helpers.Shader LightProbeShader;
        private static Helpers.Shader DebugShader;

        private static void Main()
        {
            GlfwWindowing.RegisterPlatform();
            GlfwInput.RegisterPlatform();

            UserParams.OnNChanged += OnNChanged;

            var options = WindowOptions.Default;
            options.Size = new Vector2D<int>(1280, 720);
            options.Title = "CENG469 HW2";
            window = Window.Create(options);

            window.Load += OnLoad;
            window.Render += OnRender;
            window.Update += OnUpdate;
            window.FramebufferResize += OnFramebufferResize;
            window.Closing += OnClose;

            window.Run();

            window.Dispose();
        }

        private static void OnUpdate(double obj)
        {
            UserParams.OnUpdate(obj, Keyboard);
        }

        private static void OnLoad()
        {
            IInputContext input = window.CreateInput();
            Keyboard = input.Keyboards[0];
            Keyboard.KeyDown += Keyboard_KeyDown;
            input.Mice[0].Cursor.CursorMode = CursorMode.Raw;
            input.Mice[0].MouseMove += OnMouseMove;

            Gl = GL.GetApi(window);

            MirrorShader = new Helpers.Shader(Gl, "Shaders/mirror.vert", "Shaders/mirror.frag");
            GlassShader = new Helpers.Shader(Gl, "Shaders/glass.vert", "Shaders/glass.frag");
            LightProbeShader = new Helpers.Shader(Gl, "Shaders/light_probe.vert", "Shaders/light_probe.frag");
            DebugShader = new Helpers.Shader(Gl, "Shaders/debug.vert", "Shaders/debug.frag");

            Mesh = new Model(Gl, "models/teapot.obj").Meshes[0];
            DebugMesh = new Model(Gl, "models/sphere.obj").Meshes[0];

            HDRI = new HDRTexture(Gl, "hdrs/Thumersbach.hdr");
            CubemapTexture = new CubemapTexture(Gl, sideWidth, sideHeight);
            Skybox = new Skybox(Gl, HDRI, CubemapTexture);
            MedianCut = new MedianCut(Gl, HDRI);
            MedianCut.UploadLightUBO(0);
            Gl.Viewport(window.Size);
        }

        private static void Keyboard_KeyDown(IKeyboard arg1, Key arg2, int arg3)
        {
            UserParams.ProcessKey(arg1, arg2, arg3);
        }

        private static void OnRender(double deltaTime)
        {
            Gl.Enable(EnableCap.DepthTest);
            Gl.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);

            var mat = Matrix4x4.CreateFromYawPitchRoll(MathHelper.DegreesToRadians(CameraYaw), 0, MathHelper.DegreesToRadians(CameraPitch));
            CameraGaze = Vector3.Transform(Vector3.UnitX, mat);
            Vector3 cameraPosition = CameraGaze * 4;

            var size = window.FramebufferSize;
            var model = Matrix4x4.Identity;
            var view = Matrix4x4.CreateLookAt(cameraPosition, Vector3.Zero, Vector3.UnitY);
            var projection = Matrix4x4.CreatePerspectiveFieldOfView(MathHelper.DegreesToRadians(60), (float)size.X / size.Y, 0.1f, 100.0f);

            var lightCount = (int)MathF.Pow(2, UserParams.AlgoN);

            Skybox.Render(view, projection);

            if (true)
            {
                DebugMesh.Bind();
                DebugShader.Use();
                DebugShader.SetUniform("uView", view);
                DebugShader.SetUniform("uProjection", projection);
                for (int i = 0; i < lightCount; i++)
                {
                    Light light = MedianCut.lightingData[UserParams.AlgoN][i];
                    var modelMat = Matrix4x4.CreateScale(0.05f) * Matrix4x4.CreateTranslation(light.Position * 3);
                    DebugShader.SetUniform("uModel", modelMat);
                    DebugShader.SetUniform("uColor", light.Color / 500000f);
                    Gl.DrawArrays(PrimitiveType.Triangles, 0, (uint)DebugMesh.Vertices.Length);
                }
            }

            Mesh.Bind();
            CubemapTexture.Bind();

            switch (UserParams.VisMode)
            {
                case VisMode.LightProbe:
                    LightProbeShader.Use();
                    SetCommonUniforms(LightProbeShader, cameraPosition, model, view, projection);
                    MedianCut.UploadLightUBO(UserParams.AlgoN);
                    LightProbeShader.SetUniform("uLightCount", lightCount);
                    break;
                case VisMode.Mirror:
                    MirrorShader.Use();
                    SetCommonUniforms(MirrorShader, cameraPosition, model, view, projection);
                    break;
                case VisMode.Glass:
                    GlassShader.Use();
                    SetCommonUniforms(GlassShader, cameraPosition, model, view, projection);
                    break;
                case VisMode.Glossy:

                    break;
                case VisMode.SpecularDisco:

                    break;
            }

            Gl.DrawArrays(PrimitiveType.Triangles, 0, (uint)Mesh.Vertices.Length);
        }

        private static void OnNChanged()
        {
            MedianCut.UploadLightUBO(UserParams.AlgoN);
        }

        private static void SetCommonUniforms(Helpers.Shader shader, Vector3 cameraPosition, Matrix4x4 model, Matrix4x4 view, Matrix4x4 projection)
        {
            shader.SetUniform("uModel", model);
            shader.SetUniform("uView", view);
            shader.SetUniform("uProjection", projection);
            shader.SetUniform("uCameraPos", cameraPosition);
        }

        private static void OnFramebufferResize(Vector2D<int> newSize)
        {
            Gl.Viewport(newSize);
        }

        private static void OnMouseMove(IMouse mouse, Vector2 position)
        {
            if (!mouse.IsButtonPressed(MouseButton.Right))
            {
                LastMousePosition = position;
                return;
            }

            var lookSensitivity = 0.1f;
            if (LastMousePosition == default)
            {
                LastMousePosition = position;
            }
            else
            {
                var xOffset = (position.X - LastMousePosition.X) * lookSensitivity;
                var yOffset = (position.Y - LastMousePosition.Y) * lookSensitivity;
                LastMousePosition = position;

                CameraYaw -= xOffset;
                CameraPitch += yOffset;
                CameraPitch = Math.Clamp(CameraPitch, -89.0f, 89.0f);
            }
        }

        private static void OnClose()
        {
            Mesh.Dispose();
            MirrorShader.Dispose();
            HDRI.Dispose();
            Skybox.Dispose();
        }
    }
}
