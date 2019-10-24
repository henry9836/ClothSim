#version 450 core

uniform float fogStart = 10.0f;
uniform float fogRange = 13.0f;
uniform vec4 fogColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);

in vec3 fragTexCoord;
in vec4 mWorldPos;

vec3 fragPos = vec3(7.0, 7.0, 7.0);

uniform vec3 camPos;

out vec4 color;

uniform samplerCube cubeSampler;

void main(){
	vec4 baseColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
   // Linear Fog
    float d = distance(fragPos, camPos);
    float e = (d - fogStart) / fogRange;
    float f = clamp(e, 0, 1);
    color = mix(texture(cubeSampler, fragTexCoord), fogColor, f);
}