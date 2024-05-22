#version 420 core

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

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform vec3 uCameraPos;

uniform int uLightCount;

layout(location=0) in vec3 vPos;
layout(location=1) in vec3 vNorm;

out vec4 color;

void main(void)
{
	vec4 pWorld = uModel * vec4(vPos, 1);
	vec3 nWorld = inverse(transpose(mat3x3(uModel))) * vNorm;

	for (int i = 0; i < uLightCount; i++) {
		Light light = lights[i];

		vec3 L = normalize((light.position * 1000.0) - vec3(pWorld));
		vec3 V = normalize(uCameraPos - vec3(pWorld));
		vec3 H = normalize(L + V);
		vec3 N = normalize(nWorld);

		float NdotL = dot(N, L); // for diffuse component
		float NdotH = dot(N, H); // for specular component

		vec3 diffuseColor = light.color * kd * max(0, NdotL);
		vec3 specularColor = light.color * ks * pow(max(0, NdotH), 100);
	
		color += vec4(diffuseColor + specularColor, 1);
	}

	color /= 50000.0;

    gl_Position = uProjection * uView * uModel * vec4(vPos, 1);
}

