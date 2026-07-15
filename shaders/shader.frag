#version 460 core
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
    vec3 ambientColor  = vec3(0.6, 0.6, 0.6);
    vec3 diffuseColor  = vec3(0.1, 0.1, 0.1);
    vec3 specularColor = vec3(0.2, 0.2, 0.2);
    float shininess    = 16.0;
    vec4 texColor = texture(ourTexture, texCord);
    vec3 ambient = lightColor * ambientColor;
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightColor * (diff * diffuseColor);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = lightColor * (spec * specularColor);
    vec3 result = (ambient + diffuse + specular) * texColor.rgb;
    FragColor = vec4(result, texColor.a);
}