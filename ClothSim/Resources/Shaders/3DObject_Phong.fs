#version 450 core

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;

out vec4 color;

uniform sampler2D texture_diffuse1;
uniform float ambientStr = 0.05f;
uniform vec3 ambientColor = vec3(1.0f, 1.0f, 1.0f);

void main()
{ 
   vec3 ambient = ambientStr * ambientColor;

   color = vec4(ambient, 1.0f) * (texture(texture_diffuse1, fragTexCoord));
}