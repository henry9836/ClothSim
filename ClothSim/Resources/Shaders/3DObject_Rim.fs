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

uniform vec3 camPos;
uniform float lightSpecStr = 1.0f;
uniform float shininess = 32.0f;
uniform int rimExponent = 4;
uniform vec3 rimColor = vec3(1.0f, 1.0f, 1.0f);


void main()
{ 
   vec3 ambient = ambientStr * ambientColor;
   
   vec3 norm = normalize(fragNormal);
   vec3 lightDir = normalize(fragPos - lightPos);
   
   float diffuseStr = max(dot(norm, -lightDir), 0.0f);
   vec3 diffuse = diffuseStr * lightColor;

   vec3 negViewDir = normalize(camPos - fragPos);
   vec3 halfwayVec = normalize(-lightDir + negViewDir);
   float spec = pow(max(dot(norm, halfwayVec), 0.0f), shininess);
   vec3 specular = lightSpecStr * spec * lightColor;
   
   float rimFactor = 1.0 - dot(norm, negViewDir);
   rimFactor = smoothstep(0.0, 1.0, rimFactor);
   rimFactor = pow(rimFactor, rimExponent);
   vec3 rim = rimFactor * rimColor * lightColor;

   color = vec4(ambient + diffuse + specular + rim, 1.0f) * (texture(texture_diffuse1, fragTexCoord));
}