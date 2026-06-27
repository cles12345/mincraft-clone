#include "chunk.hpp"

Chunk::Chunk(const glm::vec3& pos)
{
    dirty = false;
    created_data = false;
    world_pos = pos;    
}

void Chunk::create_data(int seed)
{
    dirty = true;
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
                    data[x][z][y] = BlockType::STONE_TYPE;
                else if (y < height)
                    data[x][z][y] = BlockType::DIRT_TYPE;
                else if (y == height)
                    data[x][z][y] = BlockType::GRASS_TYPE;
                else
                    data[x][z][y] = BlockType::NONE;
            }
        }
    }
    created_data = true;
}

void Chunk::build_mesh(std::unordered_map<glm::ivec2, Chunk>& chunks)
{
    glm::ivec2 pos_r(static_cast<int>(world_pos.x) + CHUNK_WIDTH, static_cast<int>(world_pos.z));
    glm::ivec2 pos_l(static_cast<int>(world_pos.x) - CHUNK_WIDTH, static_cast<int>(world_pos.z));
    glm::ivec2 pos_f(static_cast<int>(world_pos.x),               static_cast<int>(world_pos.z) + CHUNK_DEPTH);
    glm::ivec2 pos_b(static_cast<int>(world_pos.x),               static_cast<int>(world_pos.z) - CHUNK_DEPTH);

    vertices_opaque.clear();
    indices_opaque.clear();
    vertices_transparent.clear();
    indices_transparent.clear();
    vao_opaque.count = 0;
    vao_transparent.count = 0;
    Chunk* right = (chunks.count(pos_r) && chunks[pos_r].created_data) ? &chunks[pos_r] : nullptr;
    Chunk* left  = (chunks.count(pos_l) && chunks[pos_l].created_data) ? &chunks[pos_l] : nullptr;
    Chunk* front = (chunks.count(pos_f) && chunks[pos_f].created_data) ? &chunks[pos_f] : nullptr;
    Chunk* back  = (chunks.count(pos_b) && chunks[pos_b].created_data) ? &chunks[pos_b] : nullptr;

    for (size_t x = 0; x < CHUNK_WIDTH; x++)
    {
        for (size_t z = 0; z < CHUNK_DEPTH; z++)
        {
            for (size_t y = 0; y < CHUNK_HEIGHT; y++)
            {
                BlockType current = data[x][z][y];
                if (current == NONE) continue;                
                glm::vec3 pos(x, y, z);

                if (x + 1 >= CHUNK_WIDTH) 
                {
                    BlockType neighbor = (right != nullptr) ? right->data[0][z][y] : BlockType::NONE;
                    if (utill::should_reveal_face(current, neighbor)) add_face(RIGHT, pos);
                } 
                else 
                {
                    if (utill::should_reveal_face(current, data[x + 1][z][y])) add_face(RIGHT, pos);
                }
                if (x == 0)
                {
                    BlockType neighbor = (left != nullptr) ? left->data[CHUNK_WIDTH - 1][z][y] : BlockType::NONE;
                    if (utill::should_reveal_face(current, neighbor)) add_face(LEFT, pos);
                }
                else 
                {
                    if (utill::should_reveal_face(current, data[x - 1][z][y])) add_face(LEFT, pos);
                }

                if (y + 1 >= CHUNK_HEIGHT) 
                {
                    add_face(TOP, pos);
                } 
                else 
                {
                    if (utill::should_reveal_face(current, data[x][z][y + 1])) add_face(TOP, pos);
                }

                if (y == 0) 
                {
                    add_face(BOTTOM, pos);
                } 
                else
                {
                    if (utill::should_reveal_face(current, data[x][z][y - 1])) add_face(BOTTOM, pos);
                }

                if (z + 1 >= CHUNK_DEPTH) 
                {
                    BlockType neighbor = (front != nullptr) ? front->data[x][0][y] : BlockType::NONE;
                    if (utill::should_reveal_face(current, neighbor)) add_face(FRONT, pos);
                } 
                else 
                {
                    if (utill::should_reveal_face(current, data[x][z + 1][y])) add_face(FRONT, pos);
                }

                if (z == 0) 
                {
                    BlockType neighbor = (back != nullptr) ? back->data[x][CHUNK_DEPTH - 1][y] : BlockType::NONE;
                    if (utill::should_reveal_face(current, neighbor)) add_face(BACK, pos);
                }
                else 
                {
                    if (utill::should_reveal_face(current, data[x][z - 1][y])) add_face(BACK, pos);
                }
            }
        }
    }

    vao_opaque.bind();
    vbo_opaque.bind();
    ebo_opaque.bind();
    vao_opaque.set_layout(0, 3, FLOAT);
    vao_opaque.set_layout(1, 3, FLOAT);
    vao_opaque.set_layout(2, 2, FLOAT);
    vbo_opaque.send_buffer(vertices_opaque.data(), vertices_opaque.size());
    ebo_opaque.send_buffer(indices_opaque.data(), indices_opaque.size() * sizeof(unsigned int));

    vao_transparent.bind();
    vbo_transparent.bind();
    ebo_transparent.bind();
    vao_transparent.set_layout(0, 3, FLOAT);
    vao_transparent.set_layout(1, 3, FLOAT);
    vao_transparent.set_layout(2, 2, FLOAT);
    vbo_transparent.send_buffer(vertices_transparent.data(), vertices_transparent.size());
    ebo_transparent.send_buffer(indices_transparent.data(), indices_transparent.size() * sizeof(unsigned int));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    dirty = false;
}

void Chunk::add_face(Face face, glm::vec3 pos)
{
    float norm[3] = {0};
    auto uv = get_tile(face, data[(int)pos.x][(int)pos.z][(int)pos.y]);
    float u0 = uv[0];
    float v0 = uv[1];
    float u1 = u0 + (1.0f / ATLAS_COLS);
    float v1 = v0 + (1.0f / ATLAS_ROWS);

    std::vector<Vertex>& vertices = utill::is_transparent(data[(int)pos.x][(int)pos.z][(int)pos.y]) ? vertices_transparent : vertices_opaque;
    std::vector<unsigned int>& indices = utill::is_transparent(data[(int)pos.x][(int)pos.z][(int)pos.y]) ? indices_transparent : indices_opaque;

    switch (face)
    {
        case TOP:
            pos.y += 1.0f;
            norm[0] = 0.0f;
            norm[1] = 1.0f;
            norm[2] = 0.0f;
            vertices.push_back({pos.x, pos.y, pos.z, norm[0], norm[1], norm[2], u0, v0});
            vertices.push_back({pos.x+1.0f, pos.y, pos.z, norm[0], norm[1], norm[2], u1, v0});
            vertices.push_back({pos.x+1.0f, pos.y, pos.z+1.0f, norm[0], norm[1], norm[2], u1, v1});
            vertices.push_back({pos.x, pos.y, pos.z+1.0f, norm[0], norm[1], norm[2], u0, v1});
            break;  
        case BOTTOM:
            norm[0] = 0.0f;
            norm[1] = -1.0f;
            norm[2] = 0.0f;
            vertices.push_back({pos.x, pos.y, pos.z, norm[0], norm[1], norm[2], u0, v0});
            vertices.push_back({pos.x+1, pos.y, pos.z, norm[0], norm[1], norm[2], u1, v0});
            vertices.push_back({pos.x+1, pos.y, pos.z+1, norm[0], norm[1], norm[2], u1, v1});
            vertices.push_back({pos.x, pos.y, pos.z+1, norm[0], norm[1], norm[2], u0, v1});
            break;  
        case RIGHT:
            pos.x += 1.0f;
            norm[0] = 1.0f;
            norm[1] = 0.0f;
            norm[2] = 0.0f;
            vertices.push_back({pos.x, pos.y,   pos.z,   norm[0], norm[1], norm[2], u0, v0});
            vertices.push_back({pos.x, pos.y+1, pos.z,   norm[0], norm[1], norm[2], u0, v1});
            vertices.push_back({pos.x, pos.y+1, pos.z+1, norm[0], norm[1], norm[2], u1, v1});
            vertices.push_back({pos.x, pos.y,   pos.z+1, norm[0], norm[1], norm[2], u1, v0});
            break;  
        case LEFT:
            norm[0] = -1.0f;
            norm[1] = 0.0f;
            norm[2] = 0.0f;
            vertices.push_back({pos.x, pos.y,   pos.z,   norm[0], norm[1], norm[2], u1, v0});
            vertices.push_back({pos.x, pos.y+1, pos.z,   norm[0], norm[1], norm[2], u1, v1});
            vertices.push_back({pos.x, pos.y+1, pos.z+1, norm[0], norm[1], norm[2], u0, v1});
            vertices.push_back({pos.x, pos.y,   pos.z+1, norm[0], norm[1], norm[2], u0, v0});
            break;
        case FRONT:
            pos.z += 1.0f;
            norm[0] = 0.0f;
            norm[1] = 0.0f;
            norm[2] = 1.0f;
            vertices.push_back({pos.x, pos.y, pos.z, norm[0], norm[1], norm[2], u0, v0});
            vertices.push_back({pos.x+1.0f, pos.y, pos.z, norm[0], norm[1], norm[2], u1, v0});
            vertices.push_back({pos.x+1.0f, pos.y+1.0f, pos.z, norm[0], norm[1], norm[2], u1, v1});
            vertices.push_back({pos.x, pos.y+1.0f, pos.z, norm[0], norm[1], norm[2], u0, v1});
            break;  
        case BACK:
            norm[0] = 0.0f;
            norm[1] = 0.0f;
            norm[2] = -1.0f;
            vertices.push_back({pos.x+1.0f, pos.y, pos.z, norm[0], norm[1], norm[2], u0, v0});
            vertices.push_back({pos.x, pos.y, pos.z, norm[0], norm[1], norm[2], u1, v0});
            vertices.push_back({pos.x, pos.y+1.0f, pos.z, norm[0], norm[1], norm[2], u1, v1});
            vertices.push_back({pos.x+1.0f, pos.y+1.0f, pos.z, norm[0], norm[1], norm[2], u0, v1});
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
    case GLASS_TYPE:
        tile_index = 5;
        break;
    
    default: assert(false && "invalid type to get the tile");
    }

    int col = tile_index % ATLAS_COLS;
    int row = tile_index / ATLAS_COLS;
    float u = col * (1.0f / ATLAS_COLS);
    float v = 1.0f - ((row + 1) * (1.0f / ATLAS_ROWS));

    return { u, v };
}

void Chunk::draw_opaque(Shader& shader)
{
    if (vertices_opaque.empty()) return;
    vao_opaque.bind();
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), world_pos);
    shader.set_uniform(model, "model");
    glDrawElements(GL_TRIANGLES, ebo_opaque.indices_count, GL_UNSIGNED_INT, 0);
}

void Chunk::draw_transparent(Shader& shader)
{
    if (vertices_transparent.empty()) return;
    vao_transparent.bind();
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), world_pos);
    shader.set_uniform(model, "model");
    glDrawElements(GL_TRIANGLES, ebo_transparent.indices_count, GL_UNSIGNED_INT, 0);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

namespace utill
{
    inline bool is_transparent(BlockType type)
    {
        switch (type)
        {
            case BlockType::WATER_TYPE: return true;
            case BlockType::GLASS_TYPE: return true;
            default: return false;
        }
    }

    inline bool should_reveal_face(BlockType current, BlockType neighbor)
    {
        if (current == BlockType::NONE) return false;
        
        if (utill::is_transparent(current)) {
            return neighbor == BlockType::NONE; 
        }
        return neighbor == BlockType::NONE || utill::is_transparent(neighbor);
    }

    bool is_breakable(BlockType type)
    {
        switch (type)
        {
            case BlockType::NONE: return false;
            case BlockType::WATER_TYPE: return false;
            default: return true;
        }
    }
}