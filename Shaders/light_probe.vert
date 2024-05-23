#version 420 core

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

layout(location=0) in vec3 vPos;
layout(location=1) in vec3 vNorm;

out vec4 fragWorldPos;
out vec3 fragWorldNor;

void main(void)
{
	fragWorldPos = uModel * vec4(vPos, 1);
	fragWorldNor = inverse(transpose(mat3x3(uModel))) * vNorm;

    gl_Position = uProjection * uView * uModel * vec4(vPos, 1);
}

