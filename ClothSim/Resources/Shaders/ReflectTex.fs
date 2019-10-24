#version 450 core

in vec3 fragNormal;
in vec3 fragPos;
in vec2 fragTexCoord;

out vec4 color;

uniform vec3 camPos;
uniform sampler2D tex;
uniform samplerCube skybox;

vec4 texColor;

void main()
{
	vec3 norm = normalize(fragNormal);
	vec3 viewDir = normalize(fragPos - camPos);
	vec3 reflectDir = reflect(viewDir, norm);
	texColor = texture(tex, fragTexCoord);
	color = mix(texColor, vec4(texture(skybox, reflectDir).rgb, 1.0f), 0.35f);
}