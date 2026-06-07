#pragma once
#include <iostream>
#include <glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/hash.hpp>
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
#include "utill.hpp"

#define PLAYER_SPEED 5.0f

#define RENDER_DISTANCE_CHUNKS 4
#define RENDER_DISTANCE (RENDER_DISTANCE_CHUNKS * CHUNK_WIDTH + CHUNK_WIDTH)

namespace utill
{
    std::string world_data_to_string(BlockType (&data)[CHUNK_WIDTH][CHUNK_DEPTH][CHUNK_HEIGHT]);
}

class Game
{
    public:
        GLFWwindow* window = nullptr;
        Shader *shader = nullptr;
        Camera cam;
        float delta_time = 0;
        float last_frame = 0;
        std::unordered_map<glm::ivec2, Chunk> chunks;
        Texture *texture = nullptr;

        Game();
        void game_loop();
        void check_events();
        void clear();
        void calculate_delta_time();
        void calculate_camera_pos();
        void update();
        void save_chunk(glm::ivec2 pos);
        void load_chunk(glm::ivec2 pos);
};