#version 450 core

in vec4 mWorldPos;
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragPos;
in vec2 fragTexCoord;

out vec4 color;

uniform vec3 camPos;
uniform sampler2D tex;
uniform samplerCube skybox;
uniform vec4 fogColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
uniform float fogStart = 10.0f;
uniform float fogRange = 30.0f;

vec4 texColor;
vec4 fogColorOverlay;
vec4 baseTextureColor;

void main()
{
	


	vec3 norm = normalize(fragNormal);
	vec3 viewDir = normalize(fragPos - camPos);
	vec3 reflectDir = reflect(viewDir, norm);
	texColor = texture(tex, fragTexCoord);
	
	vec4 baseColor = texColor;
	// Linear Fog
    float d = distance(fragPos, camPos);
    float e = (d - fogStart) / fogRange;
    float f = clamp(e, 0, 1);
    fogColorOverlay = mix(vec4(texture(baseColor)), fogColor, f);
	
	baseTextureColor = mix(texColor, vec4(texture(skybox, reflectDir).rgb, 1.0f), 0.35f);
	color = mix(baseTextureColor, fogColorOverlay, f);
}