#version 330 core
out vec4 FragColor;
in vec2 texCord;
in vec3 Normal;
in vec3 fragPos;

uniform vec3 lightPos;  
uniform sampler2D ourTexture;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main()
{
    vec4 texColor = texture(ourTexture, texCord);
    vec3 lightDir = normalize(lightPos - fragPos);  
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = 2 * spec * lightColor;  
    FragColor = vec4(texColor.rgb * (0.25 + diffuse + specular), texColor.a);    
} 
