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
#include <algorithm>
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
#include "skybox.hpp"
#include "frustum.hpp"

constexpr float PLAYER_SPEED = 20.0f;

constexpr int RENDER_DISTANCE_CHUNKS = 32;
constexpr int RENDER_DISTANCE = RENDER_DISTANCE_CHUNKS * CHUNK_WIDTH;

#define ZPREPASS 0

namespace utill
{
    std::vector<uint8_t> world_data_to_uint8(BlockType (&data)[CHUNK_WIDTH][CHUNK_DEPTH][CHUNK_HEIGHT]);
    inline glm::vec3 world_pos_to_chunk_pos(glm::vec3 pos, glm::ivec2 current_chunk);
    inline glm::ivec2 world_pos_to_chunk_index(glm::vec3 pos);
}

class Game
{
    public:
        GLFWwindow* window = nullptr;
        Frustum frustum;
        Shader *shader = nullptr;
#if ZPREPASS
        Shader *zprepass_shader = nullptr;
#endif
        Skybox *skybox = nullptr;
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
        BlockType current_block = GLASS_TYPE;
        size_t loaded_per_frame = 3;
        size_t unloaded_per_frame = 3;

        Game();
        void game_loop();
        void check_events();
        void clear();
        void calculate_delta_time();
        void calculate_camera_pos();
        void update();
        void save_chunk(glm::ivec2 pos);
        void load_chunks();
        void unload_chunks();
        void create_chunks();
        void load_chunk(glm::ivec2 pos);
        void unload_far_chunks();
        bool change_block(int i, BlockType type);
        ~Game();
};