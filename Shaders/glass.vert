#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNorm;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform vec3 uCameraPos;

out vec3 reflected;

void main()
{
    vec3 world_pos = vec3(uModel * vec4(vPos, 1.0));
    vec3 towards_vertex = world_pos - uCameraPos;
    reflected = refract(towards_vertex, vec3(inverse(transpose(mat3(uModel))) * vNorm), 1.0 / 1.52);

    gl_Position = uProjection * uView * uModel *  vec4(vPos, 1.0);
}