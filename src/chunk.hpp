#pragma once
#include <vector>
#include "block.hpp"

#define CHUNK_WIDTH 10
#define CHUNK_HEIGHT 10
#define CHUNK_DEPTH 10

class Chunk
{
    public:
        std::vector<BlockType> data;

        Chunk();
};