#pragma once 
#include <glad.h>
#include <glfw3.h>
#include <stdint.h>

struct Vertex 
{
    uint8_t x, y, z;
    uint8_t normal;
    float u, v;
    Vertex(uint8_t x, uint8_t y, uint8_t z, uint8_t normal, float u, float v) : x(x), y(y), z(z), normal(normal), u(u), v(v) {}
};

struct CharVertex
{
    float x,y;
    float r, g, b;
    float u, v;

    CharVertex(float x, float y, float r, float g, float b, float u, float v) : x(x), y(y), r(r), g(g), b(b), u(u), v(v) {}
};

class VBO
{
    public:
        unsigned int id;
       
        VBO(const float vertices[], size_t vertex_size);
        VBO();
        VBO(const VBO&) = delete;
        VBO& operator=(const VBO&) = delete;
        VBO(VBO&& other) noexcept;
        VBO& operator=(VBO&& other) noexcept;
        void send_buffer(const Vertex vertices[], unsigned int elements);
        void send_buffer(const CharVertex vertices[], unsigned int elements);
        void send_buffer(const float vertices[], size_t size);
        void bind();
        ~VBO();
};