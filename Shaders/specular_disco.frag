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

vec3 ks = vec3(0.0025, 0.0025, 0.0025);   // specular reflectance coefficient

in vec4 fragWorldPos;
in vec3 fragWorldNor;
uniform float uSpecularEnabled;

out vec4 FragColor;
uniform vec3 uCameraPos;

uniform int uLightCount;
uniform mat4 uSkyboxRot;

void main(void)
{
	vec4 color = vec4(0, 0, 0, 1);
	for (int i = 0; i < uLightCount; i++) {
		Light light = lights[i];

		vec3 L = normalize(vec3(uSkyboxRot * vec4(light.position, 1.0f)));
		vec3 V = normalize(uCameraPos - vec3(fragWorldPos));
		vec3 H = normalize(L + V);
		vec3 N = normalize(fragWorldNor);

		float NdotL = dot(N, L);
		float NdotH = dot(N, H);

		vec3 specularColor = light.color * ks * pow(max(0, NdotH), 500) * uSpecularEnabled;
	
		color += vec4(specularColor, 0);
	}

	FragColor = log(color + EPSILON);
}
