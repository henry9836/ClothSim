#version 450 core

in vec2 fragTexCoord;

out vec4 color;

uniform sampler2D texture_diffuse1;

void main()
{
   color = texture(texture_diffuse1, fragTexCoord);
}