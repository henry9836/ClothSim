#version 450 core

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;

out vec4 color;

uniform sampler2D texture_diffuse1;
uniform float ambientStr = 0.05f;
uniform vec3 ambientColor = vec3(1.0f, 1.0f, 1.0f);
uniform vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
uniform vec3 lightPos = vec3(-2.0f, 6.0f, 3.0f);

void main()
{ 
   vec3 ambient = ambientStr * ambientColor;
   
   vec3 norm = normalize(fragNormal);
   vec3 lightDir = normalize(fragPos - lightPos);
   
   float diffuseStr = max(dot(norm, -lightDir), 0.0f);
   vec3 diffuse = diffuseStr * lightColor;

   color = vec4(ambient + diffuse, 1.0f) * (texture(texture_diffuse1, fragTexCoord));
}