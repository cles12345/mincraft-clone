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
#include <vector>
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

constexpr float PLAYER_SPEED = 20.0f;

constexpr int RENDER_DISTANCE_CHUNKS = 20;
constexpr int RENDER_DISTANCE = RENDER_DISTANCE_CHUNKS * CHUNK_WIDTH;

namespace utill
{
    std::vector<uint8_t> world_data_to_uint8(BlockType (&data)[CHUNK_WIDTH][CHUNK_DEPTH][CHUNK_HEIGHT]);
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
        std::vector<glm::ivec2> to_load;
        std::vector<glm::ivec2> to_unload;
        Texture *texture = nullptr;
        uint32_t seed = 0;
        glm::ivec2 last_chunk = {0, 0};
        bool there_chunks_left_to_load = true;
        bool there_chunks_left_to_unload = true;
        bool there_chunks_left_to_create = true;

        Game();
        void game_loop();
        void check_events();
        void clear();
        void calculate_delta_time();
        void calculate_camera_pos();
        void update();
        void save_chunk(glm::ivec2 pos);
        void load_3chunks();
        void unload_3chunks();
        void create_chunks();
        void load_chunk(glm::ivec2 pos);
        void unload_far_chunks();
};