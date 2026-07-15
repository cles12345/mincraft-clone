#version 460 core
layout (location = 0) in uint aPackedData;
layout (location = 1) in vec2 aTexCord;
out vec2 texCord;
out vec3 Normal;
out vec3 fragPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
    uint x = aPackedData & 0xFFu;
    uint y = (aPackedData >> 8) & 0xFFu;
    uint z = (aPackedData >> 16) & 0xFFu;
    uint norm = (aPackedData >> 24) & 0xFFu;
    vec3 position = vec3(x, y, z);
    vec4 worldPos = model * vec4(position, 1.0);
    gl_Position =  projection * view * worldPos;
    fragPos = worldPos.xyz;
    vec3 normal = (vec3(norm) / 127.5) - 1.0;
    Normal = normalize(mat3(transpose(inverse(model))) * normal);
    texCord = aTexCord;
}