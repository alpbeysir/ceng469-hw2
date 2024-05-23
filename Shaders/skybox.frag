#version 330 core
#define EPSILON 0.00001
out vec4 FragColor;

in vec3 tc;

uniform samplerCube skybox;

void main()
{    
    FragColor = log(texture(skybox, tc) + EPSILON);
}