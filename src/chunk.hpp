#pragma once
#include <vector>
#include <ctime>
#include <array>
#include "vao.hpp"
#include "vbo.hpp"  
#include "ebo.hpp"
#include "shader.hpp"

#define CHUNK_WIDTH 10
#define CHUNK_HEIGHT 255
#define CHUNK_DEPTH 10

#define ATLAS_COLS 16
#define ATLAS_ROWS 16

enum Face : uint8_t
{
    TOP,
    BOTTOM,
    RIGHT,
    LEFT,
    FRONT,
    BACK
};

enum BlockType
{
    NONE,
    GRASS_TYPE,
    STONE_TYPE,
    DIRT_TYPE
};

class Chunk
{
    public:
        BlockType data[CHUNK_WIDTH][CHUNK_DEPTH][CHUNK_HEIGHT] = {};
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        VAO vao = VAO(sizeof(Vertex));
        VBO vbo = VBO();
        EBO ebo = EBO();
        bool dirty = true;
        glm::vec3 world_pos = glm::vec3(0.0f, 0.0f, 0.0f);

        Chunk(const glm::vec3& pos);
        Chunk() = default;
        void build_mesh();
        void add_face(Face face, const glm::vec3& pos);
        std::array<float, 2> get_tile(Face face, BlockType type);
        void draw(Shader& shader);
};