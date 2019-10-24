#version 450 core

layout (location = 0) in vec3 position;

out vec3 fragTexCoord;

uniform mat4 proj_calc;

void main()
{
    
    gl_Position = proj_calc * vec4(position, 1.0f);

    fragTexCoord = position;
}