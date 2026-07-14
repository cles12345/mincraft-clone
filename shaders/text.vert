#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCord;
out vec3 ourColor;
out vec2 texCord;
uniform mat4 projection;
void main()
{
    gl_Position = projection * vec4(aPos.x, aPos.y, 0.0, 1.0);
    ourColor = aColor;
    texCord = aTexCord;
}