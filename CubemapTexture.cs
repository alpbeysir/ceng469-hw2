using Silk.NET.Assimp;
using Silk.NET.OpenGL;
using System;

namespace HW2
{
    public class CubemapTexture : IDisposable
    {
        private readonly uint _handle;
        private readonly GL _gl;

        public uint Handle => _handle;
        public TextureUnit Unit { get; private set; }

        public uint Width { get; }

        public uint Height { get; }

        public unsafe CubemapTexture(GL gl, uint width, uint height)
        {
            _gl = gl;
            Width = width;
            Height = height;

            _handle = _gl.GenTexture();
            Bind();

            for (uint i = 0; i < 6; i++)
            {
                _gl.TexImage2D((TextureTarget)((uint)TextureTarget.TextureCubeMapPositiveX + i),
                   0,
                   (int)InternalFormat.Rgba32f,
                   width,
                   height,
                   0,
                   PixelFormat.Rgba,
                   PixelType.Float,
                   null);
            }

            SetParameters();
        }

        private void SetParameters()
        {
            _gl.TexParameter(TextureTarget.TextureCubeMap, TextureParameterName.TextureWrapR, (int)GLEnum.ClampToEdge);
            _gl.TexParameter(TextureTarget.TextureCubeMap, TextureParameterName.TextureWrapS, (int)GLEnum.ClampToEdge);
            _gl.TexParameter(TextureTarget.TextureCubeMap, TextureParameterName.TextureWrapT, (int)GLEnum.ClampToEdge);
            _gl.TexParameter(TextureTarget.TextureCubeMap, TextureParameterName.TextureMinFilter, (int)GLEnum.Linear);
            _gl.TexParameter(TextureTarget.TextureCubeMap, TextureParameterName.TextureMagFilter, (int)GLEnum.Linear);
        }

        public void Bind(TextureUnit unit = TextureUnit.Texture0)
        {
            _gl.ActiveTexture(unit);
            _gl.BindTexture(TextureTarget.TextureCubeMap, _handle);
            Unit = unit;
        }

        public void Dispose()
        {
            _gl.DeleteTexture(_handle);
        }
    }
}
