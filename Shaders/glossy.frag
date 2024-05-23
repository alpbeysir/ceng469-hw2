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

vec3 kd = vec3(0.000011, 0.000011, 0.000011);     // diffuse reflectance coefficient
vec3 ks = vec3(0.00025, 0.00025, 0.00025);   // specular reflectance coefficient
float ka = 0.1f;

in vec4 fragWorldPos;
in vec3 fragWorldNor;

out vec4 FragColor;
uniform vec3 uCameraPos;
uniform float uSpecularEnabled;

uniform int uLightCount;
uniform mat4 uSkyboxRot;

in vec3 reflected;

uniform samplerCube skybox;

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

		vec3 diffuseColor = light.color * kd * max(0, NdotL);
		vec3 specularColor = light.color * ks * pow(max(0, NdotH), 15) * uSpecularEnabled;
	
		color += vec4(diffuseColor + specularColor, 0);
	}

	FragColor = log(texture(skybox, reflected) + (color * ka) + EPSILON);
}
