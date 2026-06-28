#pragma once
#include <string>
#include "vao.hpp"
#include "vbo.hpp"
#include "ebo.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include <stb_image.h>

class Skybox
{
    public:
        VAO vao = VAO(12);
        VBO vbo;
        EBO ebo;
        Shader shader = Shader("shaders/skybox.vert", "shaders/skybox.frag");
        unsigned int texture;

        Skybox();
        ~Skybox();
        void draw(Camera& camera);
};