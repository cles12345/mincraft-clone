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
        Mesh *grass_mesh = nullptr;
        Mesh *stone_mesh = nullptr;
        Shader *shader = nullptr;
        Shader *zshader = nullptr;
        Mesh *zmesh = nullptr;
        Camera cam;
        float delta_time = 0;
        float last_frame = 0;
        std::vector<Block> grass_blocks;
        std::vector<Block> stone_blocks;
        std::vector<Chunk> chunks;
        std::vector<glm::vec3> grass_block_pos;
        std::vector<glm::vec3> stone_block_pos;
        const bool ZPREPASS = false;

        Game();
        void game_loop();
        void check_events();
        void clear();
        void calculate_delta_time();
        void calculate_camera_pos();
        void update();
};