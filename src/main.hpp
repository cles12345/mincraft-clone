#pragma once
#include <iostream>
#include <glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#include <cmath>
#include "shader.hpp"
#include "mesh.hpp"
#include "texture.hpp"
#include "camera.hpp"
#include "block.hpp"
#include "chunk.hpp"

class Game
{
    public:
        GLFWwindow* window = nullptr;
        Mesh *mesh;
        Shader *shader;
        Shader *zshader;
        Mesh *zmesh;
        Camera cam;
        float delta_time = 0;
        float last_frame = 0;
        std::vector<Block> blocks;
        std::vector<Chunk> chunks;
        std::vector<glm::vec3> block_pos;
        const bool ZPREPASS = false;

        Game();
        void game_loop();
        void check_events();
        void clear();
        void calculate_delta_time();
        void calculate_camera_pos();
        void update();
        ~Game();
};