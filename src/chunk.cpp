#include "chunk.hpp"

Chunk::Chunk()
{
    srand(time(NULL));
    for (int y = 0; y < CHUNK_HEIGHT; y++)
    {
        for (int x = 0; x < CHUNK_WIDTH; x++)
        {
            for(int z = 0; z < CHUNK_DEPTH; z++)
            {
                data.push_back((BlockType)(rand() % 2));
            }
        }
    }
    
}