#version 330 core

layout(location=0) in vec3 vPos;
layout(location=1) in vec3 vNorm;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform vec3 uColor;

out vec4 color;

void main(void)
{
    color = vec4(uColor, 1);
    gl_Position = uProjection * uView * uModel * vec4(vPos, 1);
}

