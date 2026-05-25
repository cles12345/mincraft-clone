#pragma once
#include <glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "mesh.hpp"
#include "shader.hpp"
#include "texture.hpp"

class Block
{
    public:
        Texture texture;

        Block(const char* texture_path);
        void draw(Shader& shader, Mesh& mesh, glm::vec3 pos);
};