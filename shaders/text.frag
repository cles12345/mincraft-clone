#version 460 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 texCord;
uniform sampler2D ourTexture;
void main()
{
    if (texture(ourTexture, texCord).a < 0.1)
        discard;
    FragColor = vec4(ourColor, 1.0);
}