using HW2.Helpers;
using Silk.NET.OpenGL;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;
using GL = Silk.NET.OpenGL.GL;
using Shader = HW2.Helpers.Shader;

namespace HW2
{
    internal class Skybox : IDisposable
    {
        private readonly CubemapTexture cubemap;
        private readonly Shader skyboxShader;
        private readonly GL gl;
        private readonly Model cube;

        public Skybox(GL gl, HDRTexture sourceHdri, CubemapTexture cubemapTexture)
        {
            cubemap = cubemapTexture;
            skyboxShader = new(gl, "Shaders/skybox.vert", "Shaders/skybox.frag");
            cube = new(gl, "models/cube.obj");
            Shader shader = new(gl, "Shaders/fullscreen.vert", "Shaders/gen_cubemap.frag");

            uint framebuffer = gl.GenFramebuffer();
            gl.BindFramebuffer(FramebufferTarget.Framebuffer, framebuffer);

            shader.Use();

            gl.Viewport(0, 0, cubemapTexture.Width, cubemapTexture.Height);

            for (int i = 0; i < 6; i++)
            {
                gl.FramebufferTexture2D(FramebufferTarget.Framebuffer,
                                        FramebufferAttachment.ColorAttachment0,
                                        (TextureTarget)((int)GLEnum.TextureCubeMapPositiveX + i),
                                        cubemap.Handle,
                                        0);

                sourceHdri.Bind(TextureUnit.Texture0);
                shader.SetUniform("u_currentFace", i);
                shader.SetUniform("u_panorama", 0);

                gl.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);
                gl.DrawArrays(GLEnum.Triangles, 0, 3);
            }

            gl.BindFramebuffer(FramebufferTarget.Framebuffer, 0);
            this.gl = gl;
        }

        public void Render(Matrix4x4 view, Matrix4x4 projection)
        {
            gl.DepthMask(false);

            var rotated = Matrix4x4.CreateRotationY(MathHelper.DegreesToRadians(UserParams.SkyboxRotation)) * view;

            skyboxShader.Use();
            skyboxShader.SetUniform("view", rotated);
            skyboxShader.SetUniform("proj", projection);

            Mesh mesh = cube.Meshes[0];
            mesh.Bind();
            cubemap.Bind(TextureUnit.Texture0);
            skyboxShader.SetUniform("skybox", 0);

            gl.DrawArrays(GLEnum.Triangles, 0, 36);

            gl.DepthMask(true);
        }

        public void Dispose()
        {
            cubemap.Dispose();
        }
    }
}
