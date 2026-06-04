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
#include "texture.hpp"
#include "camera.hpp"
#include "chunk.hpp"
#include "vao.hpp"
#include "vbo.hpp"
#include "ebo.hpp"

#define WORLD_WIDTH 5
#define WORLD_DEPTH 5

#define PLAYER_SPEED 5.0f

class Game
{
    public:
        GLFWwindow* window = nullptr;
        Shader *shader = nullptr;
        Camera cam;
        float delta_time = 0;
        float last_frame = 0;
        std::vector<Chunk> chunks;
        Texture *texture = nullptr;

        Game();
        void game_loop();
        void check_events();
        void clear();
        void calculate_delta_time();
        void calculate_camera_pos();
        void update();
};