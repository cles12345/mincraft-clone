#pragma once
#include <glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "mesh.hpp"
#include "shader.hpp"
#include "texture.hpp"

struct BlockData
{
    Texture texture;
    Material material;
};

enum BlockType
{
    NONE,
    GRASS_TYPE,
    STONE_TYPE
};

BlockData get_block_data(BlockType type);

class Block
{
    public:
        BlockData data;

        Block() = default;
        Block(BlockType type);
        void draw(Shader& shader, Mesh& mesh, glm::vec3 pos);
};