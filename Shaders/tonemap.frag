#version 330 core

in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D hdrInput;
uniform float exposure;

float luminance(vec4 color) {
    const vec3 weights = vec3(0.2126, 0.7152, 0.0722);
    return dot(color.rgb, weights);
}

void main(void)
{
    const float gamma = 2.2;
    vec4 oneSample = exp(textureLod(hdrInput, texCoord, 10));
    float mean = luminance(oneSample);

    vec3 hdrColor = exp(texture(hdrInput, texCoord)).rgb / mean;
    hdrColor *= exposure;
  
    vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
    mapped = pow(mapped, vec3(1.0 / gamma));
  
    FragColor = vec4(mapped, 1.0);
}
