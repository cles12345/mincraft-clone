#pragma once
#include <glad.h>
#include <glfw3.h>
#include "mesh.hpp"
#include "shader.hpp"
#include <string>

class Object
{
    public:
        Object(const float *vertices, const unsigned int *indicies, size_t vertex_size, size_t indicies_size, std::string vertex_shader_path, std::string fragment_shader_path);
        Object(const float *vertices, const unsigned int *indicies, size_t vertex_size, size_t indicies_size, Shader& shader);
        void draw();

    private:
        Mesh mesh;
        Shader shader;
        float *vertices;
        unsigned int *indices;
};