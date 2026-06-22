#pragma once
#include <vector>
#include <ctime>
#include <array>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/hash.hpp>
#include "vao.hpp"
#include "vbo.hpp"  
#include "ebo.hpp"
#include "shader.hpp"
#include "FastNoiseLite.h"

constexpr int CHUNK_WIDTH = 16;
constexpr int CHUNK_HEIGHT = 256;
constexpr int CHUNK_DEPTH = 16;

constexpr int ATLAS_COLS = 16;
constexpr int ATLAS_ROWS = 16;

enum Face : uint8_t
{
    TOP,
    BOTTOM,
    RIGHT,
    LEFT,
    FRONT,
    BACK
};

enum BlockType : uint8_t
{
    NONE,
    GRASS_TYPE,
    STONE_TYPE,
    DIRT_TYPE,
    WATER_TYPE
};

class Chunk
{
    public:
        BlockType data[CHUNK_WIDTH][CHUNK_DEPTH][CHUNK_HEIGHT] = {NONE};
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        VAO vao = VAO(sizeof(Vertex));
        VBO vbo;
        EBO ebo;
        glm::vec3 world_pos = glm::vec3(0, 0, 0);
        bool dirty = false;
        bool created_data = false;
        bool added_to_load = false;
        bool added_to_unload = false;

        Chunk(const glm::vec3& pos);
        Chunk() = default;
        void create_data(int seed);
        void build_mesh(std::unordered_map<glm::ivec2, Chunk>& chunks);
        void add_face(Face face, glm::vec3 pos);
        std::array<float, 2> get_tile(Face face, BlockType type);
        void draw(Shader& shader);
        ~Chunk() = default;
};