#pragma once
#include <glad.h>
#include <glfw3.h>

enum VERTEX_TYPE 
{
    FLOAT,
    INT,
    UNSIGND_INT
};

class Mesh
{
    public:
        unsigned int VAO = 0, VBO = 0, EBO = 0, indices_count = 0, count = 0, stride = 0;

        Mesh() = default;
        Mesh(const float vertices[], size_t vertex_size, const unsigned int indicies[], size_t indicies_size, unsigned int elements, unsigned int stride);
        void bind();
        void set_layout(unsigned int location, int elements, VERTEX_TYPE type);
        ~Mesh();
};