#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform mat4 proj_calc;
uniform mat4 model;
uniform mat3 fixNormals;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragCoord;

void main(void)
{
    gl_Position = proj_calc * vec4(position, 1);
    fragPos = vec3(model * vec4(position, 1));
    fragNormal = fixNormals * normal;
    fragCoord = texCoord;
}