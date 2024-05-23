#version 420 core

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform vec3 uCameraPos;

layout(location=0) in vec3 vPos;
layout(location=1) in vec3 vNorm;

out vec4 fragWorldPos;
out vec3 fragWorldNor;
out vec3 reflected;

void main(void)
{
	fragWorldPos = uModel * vec4(vPos, 1);
	fragWorldNor = inverse(transpose(mat3x3(uModel))) * vNorm;

	vec3 world_pos = vec3(uModel * vec4(vPos, 1.0));
    vec3 towards_vertex = world_pos - uCameraPos;
    reflected = reflect(towards_vertex, vec3(inverse(transpose(mat3(uModel))) * vNorm));

    gl_Position = uProjection * uView * uModel * vec4(vPos, 1);
}

