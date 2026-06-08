#include "chunk.hpp"

Chunk::Chunk(const glm::vec3& pos)
{
    world_pos = pos;    
}

void Chunk::create_data(int seed)
{
    memset(data, 0, sizeof(data));

    FastNoiseLite noise;

    noise.SetNoiseType(FastNoiseLite::NoiseType_Value);
    noise.SetFrequency(0.01f);
    noise.SetSeed(seed);

    for (int x = 0; x < CHUNK_WIDTH; x++)
    {
        for (int z = 0; z < CHUNK_DEPTH; z++)
        {
            float noise_value = noise.GetNoise((float)x + world_pos.x, (float)z + world_pos.z);
            int height = (int)((noise_value + 1.0f) / 2.0f * CHUNK_HEIGHT);

            if (height < 0) height = 0;
            if (height >= CHUNK_HEIGHT) height = CHUNK_HEIGHT - 1;

            for (int y = 0; y < CHUNK_HEIGHT; y++)
            {
                if (y < height - 3)
                    data[x][z][y] = STONE_TYPE;
                else if (y < height)
                    data[x][z][y] = DIRT_TYPE;
                else if (y == height)
                    data[x][z][y] = GRASS_TYPE;
                else
                    data[x][z][y] = NONE;
            }
        }
    }
}

void Chunk::build_mesh(std::unordered_map<glm::ivec2, Chunk>& chunks)
{
    vertices.clear();
    indices.clear();
    Chunk* right = nullptr;
    Chunk* left = nullptr;
    Chunk* front = nullptr;
    Chunk* back = nullptr;

    glm::ivec2 Pos(world_pos.x + CHUNK_WIDTH, world_pos.z);
    if (chunks.count(Pos))
    {
        right = &chunks[Pos];
    }
    Pos.x = world_pos.x - CHUNK_WIDTH;
    if (chunks.count(Pos))
    {
        left = &chunks[Pos];
    }
    Pos.x = world_pos.x;
    Pos.y  = world_pos.z + CHUNK_DEPTH;
    if (chunks.count(Pos))
    {
        front = &chunks[Pos];
    }
    Pos.y  = world_pos.z - CHUNK_DEPTH;
    if (chunks.count(Pos))
    {
        back = &chunks[Pos];
    }
    
    for (size_t x = 0; x < CHUNK_WIDTH; x++)
    {
        for (size_t z = 0; z < CHUNK_DEPTH; z++)
        {
            for (size_t y = 0; y < CHUNK_HEIGHT; y++)
            {
                if (data[x][z][y] == NONE) continue;

                glm::vec3 pos(x, y, z);

                if (x+1 >= CHUNK_WIDTH)
                {
                    if (right == nullptr || right->data[0][z][y] == NONE)
                    {
                        add_face(RIGHT, pos);
                    }
                }
                else if(data[x+1][z][y] == NONE)
                {
                    add_face(RIGHT, pos);
                }

                if (x == 0)
                {
                    if (left == nullptr || left->data[CHUNK_WIDTH-1][z][y] == NONE)
                    {
                        add_face(LEFT, pos);
                
                    }
                }
                else if(data[x-1][z][y] == NONE)
                {
                    add_face(LEFT, pos);
                }

                if (y+1 >= CHUNK_HEIGHT || data[x][z][y+1] == NONE)
                {
                    add_face(TOP, pos);
                }
                if (y == 0 || data[x][z][y-1] == NONE)
                {
                    add_face(BOTTOM, pos);
                }

                if (z+1 >= CHUNK_DEPTH)
                {
                    if (front == nullptr || front->data[x][0][y] == NONE)
                    {
                        add_face(FRONT, pos);
                    }
                }
                else if(data[x][z+1][y] == NONE)
                {
                    add_face(FRONT, pos);
                }
                if (z == 0)
                {
                    if (back == nullptr || back->data[x][CHUNK_DEPTH-1][y] == NONE)
                    {
                        add_face(BACK, pos);
                
                    }
                }
                else if(data[x][z-1][y] == NONE)
                {
                    add_face(BACK, pos);
                }
            }
        }
    }
    vao.bind();
    ebo.bind(); 
    vbo.bind();
    vao.set_layout(0, 3, FLOAT);
    vao.set_layout(1, 3, FLOAT);
    vao.set_layout(2, 2, FLOAT);
    vbo.send_buffer(vertices.data(), vertices.size());
    ebo.send_buffer(indices.data(), indices.size() * sizeof(unsigned int));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Chunk::add_face(Face face, const glm::vec3& pos)
{
    float Pos[3] = {pos.x, pos.y, pos.z};
    float norm[3] = {0};
    auto uv = get_tile(face, data[(int)Pos[0]][(int)Pos[2]][(int)Pos[1]]);
    float u0 = uv[0];
    float v0 = uv[1];
    float u1 = u0 + (1.0f / ATLAS_COLS);
    float v1 = v0 + (1.0f / ATLAS_ROWS);

    switch (face)
    {
        case TOP:
            Pos[1] += 1.0f;
            norm[0] = 0.0f;
            norm[1] = 1.0f;
            norm[2] = 0.0f;
            vertices.push_back({Pos[0], Pos[1], Pos[2], norm[0], norm[1], norm[2], u0, v0});
            vertices.push_back({Pos[0]+1, Pos[1], Pos[2], norm[0], norm[1], norm[2], u1, v0});
            vertices.push_back({Pos[0]+1, Pos[1], Pos[2]+1, norm[0], norm[1], norm[2], u1, v1});
            vertices.push_back({Pos[0], Pos[1], Pos[2]+1, norm[0], norm[1], norm[2], u0, v1});
            break;  

        case BOTTOM:
            norm[0] = 0.0f;
            norm[1] = -1.0f;
            norm[2] = 0.0f;
            vertices.push_back({Pos[0], Pos[1], Pos[2], norm[0], norm[1], norm[2], u0, v0});
            vertices.push_back({Pos[0]+1, Pos[1], Pos[2], norm[0], norm[1], norm[2], u1, v0});
            vertices.push_back({Pos[0]+1, Pos[1], Pos[2]+1, norm[0], norm[1], norm[2], u1, v1});
            vertices.push_back({Pos[0], Pos[1], Pos[2]+1, norm[0], norm[1], norm[2], u0, v1});
            break;  

        case RIGHT:
            Pos[0] += 1.0f;
            norm[0] = 1.0f;
            norm[1] = 0.0f;
            norm[2] = 0.0f;
            vertices.push_back({Pos[0], Pos[1],   Pos[2],   norm[0], norm[1], norm[2], u0, v0});
            vertices.push_back({Pos[0], Pos[1]+1, Pos[2],   norm[0], norm[1], norm[2], u0, v1});
            vertices.push_back({Pos[0], Pos[1]+1, Pos[2]+1, norm[0], norm[1], norm[2], u1, v1});
            vertices.push_back({Pos[0], Pos[1],   Pos[2]+1, norm[0], norm[1], norm[2], u1, v0}); 
            break;  

        case LEFT:
            norm[0] = -1.0f;
            norm[1] = 0.0f;
            norm[2] = 0.0f;
            vertices.push_back({Pos[0], Pos[1],   Pos[2],   norm[0], norm[1], norm[2], u1, v0});
            vertices.push_back({Pos[0], Pos[1]+1, Pos[2],   norm[0], norm[1], norm[2], u1, v1});
            vertices.push_back({Pos[0], Pos[1]+1, Pos[2]+1, norm[0], norm[1], norm[2], u0, v1});
            vertices.push_back({Pos[0], Pos[1],   Pos[2]+1, norm[0], norm[1], norm[2], u0, v0});
            break; 

        case FRONT:
            Pos[2] += 1.0f;
            norm[0] = 0.0f;
            norm[1] = 0.0f;
            norm[2] = 1.0f;
            vertices.push_back({Pos[0], Pos[1], Pos[2], norm[0], norm[1], norm[2], u0, v0});
            vertices.push_back({Pos[0]+1, Pos[1], Pos[2], norm[0], norm[1], norm[2], u1, v0});
            vertices.push_back({Pos[0]+1, Pos[1]+1, Pos[2], norm[0], norm[1], norm[2], u1, v1});
            vertices.push_back({Pos[0], Pos[1]+1, Pos[2], norm[0], norm[1], norm[2], u0, v1});
            break;  
        case BACK:
            norm[0] = 0.0f;
            norm[1] = 0.0f;
            norm[2] = -1.0f;
            vertices.push_back({Pos[0], Pos[1], Pos[2], norm[0], norm[1], norm[2], u0, v0});
            vertices.push_back({Pos[0]+1, Pos[1], Pos[2], norm[0], norm[1], norm[2], u1, v0});
            vertices.push_back({Pos[0]+1, Pos[1]+1, Pos[2], norm[0], norm[1], norm[2], u1, v1});
            vertices.push_back({Pos[0], Pos[1]+1, Pos[2], norm[0], norm[1], norm[2], u0, v1});
            break;  

        default: assert(false && "unknow face to add");
    }

    unsigned int offset = vertices.size() - 4;
    indices.push_back(offset + 0);
    indices.push_back(offset + 1);
    indices.push_back(offset + 2);
    indices.push_back(offset + 0);
    indices.push_back(offset + 2);
    indices.push_back(offset + 3);
}

std::array<float, 2> Chunk::get_tile(Face face, BlockType type)
{
    int tile_index = 0;

    switch (type)
    {
    case STONE_TYPE:
        tile_index = 3;
        break;
    case GRASS_TYPE:
        if (face == TOP)
        {
            tile_index = 1;
            break;
        }
        else if (face == BOTTOM)
        {
            tile_index = 2;
            break;
        }
        tile_index = 0;
        break;
    case DIRT_TYPE:
        tile_index = 2;
        break;
    case WATER_TYPE:
        tile_index = 4;
        break;
    
    default: assert(false && "invalid type to get the tile");
    }

    int col = tile_index % ATLAS_COLS;
    int row = tile_index / ATLAS_COLS;
    float u = col * (1.0f / ATLAS_COLS);
    float v = 1.0f - ((row + 1) * (1.0f / ATLAS_ROWS));

    return { u, v };
}

void Chunk::draw(Shader& shader)
{
    vao.bind();
    ebo.bind();
    glm::mat4 model = glm::translate(glm::mat4(1.0f), world_pos);
    shader.set_uniform(model, "model");
    glDrawElements(GL_TRIANGLES, ebo.indices_count, GL_UNSIGNED_INT, 0);
}
