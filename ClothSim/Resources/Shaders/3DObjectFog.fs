#version 450 core

uniform float fogStart = 10.0f;
uniform float fogRange = 30.0f;
uniform vec4 fogColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);

in vec2 fragTexCoord;
in vec4 mWorldPos;
in vec3 fragPos;

out vec4 color;

uniform sampler2D texture_diffuse1;
uniform vec3 camPos;


void main()
{
   vec4 baseColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
   // Linear Fog
    float d = distance(fragPos, camPos);
    float e = (d - fogStart) / fogRange;
    float f = clamp(e, 0, 1);
    color = mix(baseColor, fogColor, f);

}