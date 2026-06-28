#pragma once 
#include <glad.h>
#include <glfw3.h>

struct Vertex 
{
    float x, y, z;
    float nx, ny, nz;
    float u, v;
};

class VBO
{
    public:
        unsigned int id;
       
        VBO(const float vertices[], size_t vertex_size);
        VBO();
        void send_buffer(const Vertex vertices[], unsigned int elements);
        void send_buffer(const float vertices[], size_t size);
        void bind();
        ~VBO();
};