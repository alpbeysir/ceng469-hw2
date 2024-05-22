#version 330 core
layout (location = 0) in vec3 vPos;

uniform mat4 view;
uniform mat4 proj;

out vec3 tc;

void main(void) 
{
	tc = vPos;
	gl_Position = proj * mat4(mat3(view)) * vec4(vPos, 1.0);
}