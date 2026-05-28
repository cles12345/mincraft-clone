#version 330 core
out vec4 FragColor;
in vec2 texCord;
in vec3 Normal;
in vec3 fragPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
  
uniform Material material;
uniform vec3 lightPos;  
uniform sampler2D ourTexture;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main()
{
    vec4 texColor = texture(ourTexture, texCord);
    vec3 ambient = lightColor * material.ambient;   
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightColor * (diff * material.diffuse);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = lightColor * (spec * material.specular);  
    FragColor = vec4((ambient + diffuse + specular) * texColor.rgb, texColor.a);    
} 
