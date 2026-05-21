#pragma once
#include <glad.h>
#include <glfw3.h>
#include "mesh.hpp"
#include "shader.hpp"
#include <string>

class Object
{
    public:
        Mesh mesh;
        Shader shader;
        float *vertices;

        Object(const float *vertices, const unsigned int *indicies, size_t vertex_size, size_t indicies_size, unsigned int elements, unsigned int stride, std::string vertex_shader_path, std::string fragment_shader_path);
        Object(const float *vertices, const unsigned int *indicies, size_t vertex_size, size_t indicies_size, unsigned int elements, unsigned int stride, Shader& shader);
        void draw();
};