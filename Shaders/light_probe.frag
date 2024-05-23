#version 420 core
#define EPSILON 0.00001

struct Light {
	vec3 position;
	vec3 color;
};

layout (std140, binding = 0) uniform Lights
{
    Light lights[128];
};

vec3 kd = vec3(1, 1, 1);     // diffuse reflectance coefficient
vec3 ks = vec3(0.8, 0.8, 0.8);   // specular reflectance coefficient

in vec4 fragWorldPos;
in vec3 fragWorldNor;

out vec4 FragColor;
uniform vec3 uCameraPos;

uniform int uLightCount;

void main(void)
{
	vec4 color = vec4(0, 0, 0, 1);
	for (int i = 0; i < uLightCount; i++) {
		Light light = lights[i];

		vec3 L = normalize(light.position);
		vec3 V = normalize(uCameraPos - vec3(fragWorldPos));
		vec3 H = normalize(L + V);
		vec3 N = normalize(fragWorldNor);

		float NdotL = dot(N, L); // for diffuse component
		float NdotH = dot(N, H); // for specular component

		vec3 diffuseColor = light.color * kd * max(0, NdotL);
		vec3 specularColor = light.color * ks * pow(max(0, NdotH), 750);
	
		color += vec4(/*diffuseColor +*/ specularColor, 0);
	}

	FragColor = log(color + EPSILON);
}
