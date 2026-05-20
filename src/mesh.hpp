#pragma once
#include <glad.h>
#include <glfw3.h>

class Mesh
{
    public:
        Mesh(const float vertices[], size_t vertex_size, const unsigned int indicies[], size_t indicies_size);
        void bind();
        unsigned int vertices_count, indices_count;
        ~Mesh();
    private:
        unsigned int VAO, VBO, EBO;
};