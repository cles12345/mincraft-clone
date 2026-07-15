#version 460 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 texCord;
uniform sampler2D ourTexture;
void main()
{
    FragColor = vec4(ourColor, texture(ourTexture, texCord).a);
}