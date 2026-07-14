#include "chunk.hpp"

Chunk::Chunk(const glm::vec3& pos)
{
    dirty = false;
    created_data = false;
    world_pos = pos;    
}

void Chunk::create_data(int seed)
{
	data.reset();

    static FastNoiseLite terrain_nois_3d;
    static FastNoiseLite base_land_noise;
    static FastNoiseLite cheese_cave_noise;
    static FastNoiseLite spaghetti_cave_noise;
    static FastNoiseLite water_cave_noise;
    static int initialized_seed = -1;

    if (initialized_seed != seed)
    {
        initialized_seed = seed;
        terrain_nois_3d.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
        terrain_nois_3d.SetFrequency(0.02f);
        terrain_nois_3d.SetFractalOctaves(5);
        terrain_nois_3d.SetFractalType(FastNoiseLite::FractalType_FBm);
        terrain_nois_3d.SetSeed(seed);

        base_land_noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
        base_land_noise.SetFrequency(0.005f);
        base_land_noise.SetFractalOctaves(5);
        base_land_noise.SetFractalType(FastNoiseLite::FractalType_FBm);
        base_land_noise.SetSeed(seed + 500);

        cheese_cave_noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
        cheese_cave_noise.SetFrequency(0.025f);
        cheese_cave_noise.SetSeed(seed + 1000);

        spaghetti_cave_noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
        spaghetti_cave_noise.SetFrequency(0.03f);
        spaghetti_cave_noise.SetSeed(seed + 2000);

        water_cave_noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
        water_cave_noise.SetFrequency(0.01f);
        water_cave_noise.SetSeed(seed + 3000);
    }

    for (int x = 0; x < CHUNK_WIDTH; x++)
    {
        for (int z = 0; z < CHUNK_DEPTH; z++)
        {
            float wx = world_pos.x + x;
            float wz = world_pos.z + z;
            
            float continentalness = base_land_noise.GetNoise(wx, wz);

            float land_variation = continentalness * 35.0f;
            float local_sea_level = SEA_LEVEL + land_variation;

            float flood_chance = water_cave_noise.GetNoise(wx, wz);

            float mountain_intensity = 12.0f;
            if (continentalness > 0.0f) 
            {
                mountain_intensity += continentalness * 65.0f;
            }
            int blocks_surface_depth = 0;

            int start_y = (int)std::max((float)SEA_LEVEL, local_sea_level + mountain_intensity);

            for (int y = start_y; y >= 1; y--)
            {
                float height_pull = local_sea_level - (float)y;
                float noise3d = terrain_nois_3d.GetNoise(wx, (float)y, wz);

                float final_density = height_pull + (noise3d * mountain_intensity);

                if (final_density > 0.0f)
                {
                    bool is_cave = false;
                    
                    if (y < local_sea_level - 4)
                    {
                        float cheese_val = cheese_cave_noise.GetNoise(wx, (float)y, wz);
                        
                        float spaghetti_val = spaghetti_cave_noise.GetNoise(wx, (float)y, wz);
                        bool is_spaghetti = std::abs(spaghetti_val) < 0.08f;

                        if (cheese_val > 0.50f || is_spaghetti)
                        {
                            is_cave = true;
                        }
                    }

                    if (is_cave)
                    {
                        blocks_surface_depth = 0;

                        if (y <= SEA_LEVEL - 8 && flood_chance > 0.4f)
							data.set(x, y, z, BlockType::WATER_TYPE);
                        else
							data.set(x, y, z, BlockType::NONE);
                    }
                    else 
                    {
                        bool is_beach_zone = (y <= SEA_LEVEL + 2 && y >= SEA_LEVEL - 3);

                        if (blocks_surface_depth == 0)
                        {
                            if (is_beach_zone)
								data.set(x, y, z, BlockType::SAND_TYPE);
                            else if (y > SEA_LEVEL)
								data.set(x, y, z, BlockType::GRASS_TYPE);
                            else
								data.set(x, y, z, BlockType::STONE_TYPE);
                        }
                        else if (blocks_surface_depth < DIRT_LEVEL)
                        {
                            if (is_beach_zone)
								data.set(x, y, z, BlockType::SAND_TYPE);
                            else if (y > SEA_LEVEL)
								data.set(x, y, z, BlockType::DIRT_TYPE);
                            else
								data.set(x, y, z, BlockType::STONE_TYPE);
                        }
                        else
							data.set(x, y, z, BlockType::STONE_TYPE);

                        blocks_surface_depth++;
                    }
                }
                else
                {
                    blocks_surface_depth = 0;
                    if (y <= SEA_LEVEL)
						data.set(x, y, z, BlockType::WATER_TYPE);
                    else
						data.set(x, y, z, BlockType::NONE);
                }
            }

			data.set(x, 0, z, BlockType::BEDROCK_TYPE);
        }
    }
	dirty = true;
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
                BlockType current = data.get(x, y, z);
                if (current == NONE) continue;                
                glm::vec3 pos(x, y, z);

                if (x + 1 >= CHUNK_WIDTH) 
                {
                    BlockType neighbor = (right != nullptr) ? right->data.get(0, y, z) : BlockType::NONE;
                    if (utill::should_reveal_face(current, neighbor)) add_face(RIGHT, pos);
                } 
                else 
                {
                    if (utill::should_reveal_face(current, data.get(x + 1, y, z))) add_face(RIGHT, pos);
                }
                if (x == 0)
                {
                    BlockType neighbor = (left != nullptr) ? left->data.get(CHUNK_WIDTH - 1, y, z) : BlockType::NONE;
                    if (utill::should_reveal_face(current, neighbor)) add_face(LEFT, pos);
                }
                else 
                {
                    if (utill::should_reveal_face(current, data.get(x - 1, y, z))) add_face(LEFT, pos);
                }

                if (y + 1 >= CHUNK_HEIGHT) 
                {
                    add_face(TOP, pos);
                } 
                else 
                {
                    if (utill::should_reveal_face(current, data.get(x, y + 1, z))) add_face(TOP, pos);
                }

                if (y == 0) 
                {
                    add_face(BOTTOM, pos);
                } 
                else
                {
                    if (utill::should_reveal_face(current, data.get(x, y - 1, z))) add_face(BOTTOM, pos);
                }

                if (z + 1 >= CHUNK_DEPTH) 
                {
                    BlockType neighbor = (front != nullptr) ? front->data.get(x, y, 0) : BlockType::NONE;
                    if (utill::should_reveal_face(current, neighbor)) add_face(FRONT, pos);
                } 
                else 
                {
                    if (utill::should_reveal_face(current, data.get(x, y, z + 1))) add_face(FRONT, pos);
                }

                if (z == 0) 
                {
                    BlockType neighbor = (back != nullptr) ? back->data.get(x, y, CHUNK_DEPTH - 1) : BlockType::NONE;
                    if (utill::should_reveal_face(current, neighbor)) add_face(BACK, pos);
                }
                else 
                {
                    if (utill::should_reveal_face(current, data.get(x, y, z - 1))) add_face(BACK, pos);
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
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    dirty = false;
}

void Chunk::add_face(Face face, glm::vec3 pos)
{
    float norm[3] = {0};
    auto uv = get_tile(face, data.get((int)pos.x, (int)pos.y, (int)pos.z));
    float u0 = uv[0];
    float v0 = uv[1];
    float u1 = u0 + (1.0f / ATLAS_COLS);
    float v1 = v0 + (1.0f / ATLAS_ROWS);

    std::vector<Vertex>& vertices = utill::is_transparent(data.get((int)pos.x, (int)pos.y, (int)pos.z)) ? vertices_transparent : vertices_opaque;
    std::vector<unsigned int>& indices = utill::is_transparent(data.get((int)pos.x, (int)pos.y, (int)pos.z)) ? indices_transparent : indices_opaque;

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
    case BlockType::STONE_TYPE:
        tile_index = 3;
        break;
    case BlockType::GRASS_TYPE:
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
    case BlockType::DIRT_TYPE:
        tile_index = 2;
        break;
    case BlockType::WATER_TYPE:
        tile_index = 4;
        break;
    case BlockType::GLASS_TYPE:
        tile_index = 5;
        break;
    case BlockType::SAND_TYPE:
        tile_index = 6;
        break;
    case BlockType::BEDROCK_TYPE:
        tile_index = 7;
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
    glBindVertexArray(0);
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
    glBindVertexArray(0);
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
            case BlockType::BEDROCK_TYPE: return false;
            default: return true;
        }
    }
}