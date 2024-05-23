#version 330 core
#define EPSILON 0.00001
in vec4 color;
out vec4 FragColor;

void main(void)
{
	FragColor = log(color + EPSILON);
}
