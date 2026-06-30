#include "main.hpp"

int main(int argc, char const *argv[])
{
    Game game;
    game.game_loop();
    return 0;
}

Game::Game() : cam(45.0f, 800.0f, 600.0f)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, false);

    window = glfwCreateWindow(800, 600, "game", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create a window\n";
        glfwTerminate();
        assert(false);
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize glad\n";
        assert(false);
    }    
    glViewport(0, 0, 800, 600);
    
    srand(time(nullptr));

    if (utill::file_exist("save/seed"))
    {
        seed = utill::read_file_binary("save/seed");
    }
    else
    {
        seed = rand();
        utill::write_file_binary("save/seed", seed);
    }

    shader = new Shader("shaders/shader.vert", "shaders/shader.frag");
#if ZPREPASS
    zprepass_shader = new Shader("shaders/zprepass.vert", "shaders/zprepass.frag");
#endif
    texture = new Texture("sprite/atlas.png", GL_REPEAT, GL_REPEAT, GL_NEAREST_MIPMAP_LINEAR, GL_NEAREST);
    skybox = new Skybox;
    last_frame = glfwGetTime();

    cam.pos = glm::vec3(0.0f, 200.0f, 0.0f);

    last_chunk.x = std::floor(cam.pos.x / CHUNK_WIDTH);
    last_chunk.y = std::floor(cam.pos.z / CHUNK_DEPTH);

    int radius = RENDER_DISTANCE_CHUNKS / 2;
    for (int x = last_chunk.x - radius; x <= last_chunk.x + radius; x++)
    {
        for (int z = last_chunk.y - radius; z <= last_chunk.y + radius; z++)
        {
            glm::ivec2 pos(x * CHUNK_WIDTH, z * CHUNK_DEPTH);
            if (!chunks.count(pos))
            {
                chunks.try_emplace(pos, glm::vec3(pos.x, 0, pos.y));
            }
        }
    }
    
    there_chunks_left_to_load = true;
    there_chunks_left_to_unload = true;

    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Game::game_loop()
{
    while(!glfwWindowShouldClose(window))
    {
        clear();
        calculate_delta_time();
        calculate_camera_pos();
        update();   
    }
}

void Game::check_events()
{
    bool moved = false;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cam.move_forward(PLAYER_SPEED * delta_time);
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cam.move_backward(PLAYER_SPEED * delta_time);
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cam.move_right(PLAYER_SPEED * delta_time);
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cam.move_left(PLAYER_SPEED * delta_time);
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        cam.move_up(PLAYER_SPEED * delta_time);
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        cam.move_down(PLAYER_SPEED * delta_time);
        moved = true;
    }

    if (moved)
    {
        glm::ivec2 current_chunk = {std::floor(cam.pos.x / CHUNK_WIDTH), std::floor(cam.pos.z / CHUNK_DEPTH)};
        if (current_chunk != last_chunk)
        {
            last_chunk = current_chunk;
            unload_far_chunks();

            int radius = RENDER_DISTANCE_CHUNKS / 2;
            for (int x = current_chunk.x - radius; x <= current_chunk.x + radius; x++)
            {
                for (int z = current_chunk.y - radius; z <= current_chunk.y + radius; z++)
                {
                    glm::ivec2 pos(x * CHUNK_WIDTH, z * CHUNK_DEPTH);
                    if (!chunks.count(pos))
                    {
                        chunks.try_emplace(pos, glm::vec3(pos.x, 0, pos.y));
                    }
                }
            }
            there_chunks_left_to_load = true;
            there_chunks_left_to_unload = true;
        }
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        for (int i = 4; i >= 0; i--)
        {
            glm::vec3 pos = cam.pos + cam.direction * (float)i;

            glm::ivec2 current_chunk(utill::world_pos_to_chunk_index(pos));
            glm::ivec2 chunk_key(current_chunk.x * CHUNK_WIDTH, current_chunk.y * CHUNK_DEPTH);

            if (!chunks.count(chunk_key) || !chunks[chunk_key].created_data)
            {
                continue;
            }
            
            glm::vec3 local_pos(utill::world_pos_to_chunk_pos(pos, current_chunk));

            int block_x = (int)std::floor(local_pos.x);
            int block_y = (int)std::floor(local_pos.y);
            int block_z = (int)std::floor(local_pos.z);

            if (block_x < 0 || block_x >= CHUNK_WIDTH ||
                block_y < 0 || block_y >= CHUNK_HEIGHT ||
                block_z < 0 || block_z >= CHUNK_DEPTH)
            {
                continue;
            }

            if(utill::is_breakable(chunks[chunk_key].data[block_x][block_z][block_y]))
            {
                glm::ivec2 cam_chunk_idx = utill::world_pos_to_chunk_index(cam.pos);
                glm::vec3 local_cam_pos = utill::world_pos_to_chunk_pos(cam.pos, cam_chunk_idx);

                if (cam_chunk_idx != current_chunk) {
                    local_cam_pos.x += (cam_chunk_idx.x - current_chunk.x) * CHUNK_WIDTH;
                    local_cam_pos.z += (cam_chunk_idx.y - current_chunk.y) * CHUNK_DEPTH;
                }

                float t_target_x = (cam.direction.x > 0) ? ((float)block_x - local_cam_pos.x) / cam.direction.x 
                                                        : ((float)(block_x + 1) - local_cam_pos.x) / cam.direction.x;

                float t_target_y = (cam.direction.y > 0) ? ((float)block_y - local_cam_pos.y) / cam.direction.y 
                                                        : ((float)(block_y + 1) - local_cam_pos.y) / cam.direction.y;

                float t_target_z = (cam.direction.z > 0) ? ((float)block_z - local_cam_pos.z) / cam.direction.z 
                                                        : ((float)(block_z + 1) - local_cam_pos.z) / cam.direction.z;

                glm::ivec3 hit_normal;
                if (t_target_x > t_target_y && t_target_x > t_target_z) 
                {
                    hit_normal = utill::is_look_right(cam.direction) ? glm::ivec3(-1, 0, 0) : glm::ivec3(1, 0, 0);
                } 
                else if (t_target_y > t_target_x && t_target_y > t_target_z) 
                {
                    hit_normal = utill::is_look_up(cam.direction) ? glm::ivec3(0, -1, 0) : glm::ivec3(0, 1, 0);
                } 
                else 
                {
                    hit_normal = utill::is_look_front(cam.direction) ? glm::ivec3(0, 0, -1) : glm::ivec3(0, 0, 1);
                }

                int target_x = block_x + hit_normal.x;
                int target_y = block_y + hit_normal.y;
                int target_z = block_z + hit_normal.z;

                bool can_break = false;
                if (target_x >= 0 && target_x < CHUNK_WIDTH &&
                    target_y >= 0 && target_y < CHUNK_HEIGHT &&
                    target_z >= 0 && target_z < CHUNK_DEPTH)
                {
                    if (!utill::is_breakable(chunks[chunk_key].data[target_x][target_z][target_y]))
                    {
                        can_break = true;
                    }
                }
                else
                {
                    can_break = true;
                }

                if (can_break)
                {
                    chunks[chunk_key].data[block_x][block_z][block_y] = BlockType::NONE;
                    chunks[chunk_key].dirty = true;
                    
                    if (block_x == 0) 
                    {
                        glm::ivec2 key(chunk_key.x - CHUNK_WIDTH, chunk_key.y);
                        if (chunks.count(key)) chunks[key].dirty = true;
                    }
                    if (block_x == CHUNK_WIDTH - 1) 
                    {
                        glm::ivec2 key(chunk_key.x + CHUNK_WIDTH, chunk_key.y);
                        if (chunks.count(key)) chunks[key].dirty = true;
                    }
                    if (block_z == 0) 
                    {
                        glm::ivec2 key(chunk_key.x, chunk_key.y - CHUNK_DEPTH);
                        if (chunks.count(key)) chunks[key].dirty = true;
                    }
                    if (block_z == CHUNK_DEPTH - 1) 
                    {
                        glm::ivec2 key(chunk_key.x, chunk_key.y + CHUNK_DEPTH);
                        if (chunks.count(key)) chunks[key].dirty = true;
                    }
                    
                    there_chunks_left_to_create = true;
                    break;
                }
            }
        }
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        for (int i = 4; i >= 0; i--)
        {
           if(change_block(i, BlockType::WATER_TYPE))
           {
                break;
           }
        }
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        for (int i = 4; i >= 0; i--)
        {
           if(change_block(i, BlockType::GLASS_TYPE))
           {
                break;
           }
        }
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        for (int i = 4; i >= 0; i--)
        {
           if(change_block(i, BlockType::SAND_TYPE))
           {
                break;
           }
        }
    }
}

void Game::clear()
{
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Game::calculate_delta_time()
{
    float current_frame = glfwGetTime();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;
}

void Game::calculate_camera_pos()
{
    double cam_x, cam_y;
    glfwGetCursorPos(window, &cam_x, &cam_y);
    cam.mouse_callback(cam_x, cam_y);
}

void Game::update()
{
    check_events();
    cam.update(*shader);   

    float light_color[3] = {1.0f, 1.0f, 1.0f};
    float light_pos[3] = {cam.pos.x, cam.pos.y, cam.pos.z};

    float fps = 1.0f / delta_time;
    std::string title = "FPS: " + std::to_string(fps);
    glfwSetWindowTitle(window, title.c_str());

    skybox->draw(cam);
    
#if ZPREPASS
    glDepthFunc(GL_LESS);
    glColorMask(0, 0, 0, 0);
    cam.update(*zprepass_shader);

    for (auto& [pos, chunk] : chunks)
    {
        if (glm::distance(glm::vec3(pos.x, cam.pos.y, pos.y), cam.pos) < RENDER_DISTANCE && chunk.created_data)
        {
            chunk.draw_opaque(*zprepass_shader);
        }
    }
    glDepthFunc(GL_EQUAL);
    glColorMask(1, 1, 1, 1);
#endif
    cam.update(*shader);
    shader->set_uniform(light_color[0], light_color[1], light_color[2], "lightColor");
    shader->set_uniform(light_pos[0], light_pos[1], light_pos[2], "lightPos");
    shader->set_uniform(cam.pos[0], cam.pos[1], cam.pos[2], "viewPos");
    for (auto& [pos, chunk] : chunks)
    {
        if (!chunk.created_data && !chunk.added_to_load)
        {
            to_load.push_back(pos);
            chunk.added_to_load = true;
        }

        if (glm::distance(glm::vec3(pos.x, cam.pos.y, pos.y), cam.pos) < RENDER_DISTANCE && chunk.created_data)
        {
            chunk.draw_opaque(*shader);
        }
    }
    glDepthFunc(GL_LESS);

    for (auto& [pos, chunk] : chunks)
    {
        if (glm::distance(glm::vec3(pos.x, cam.pos.y, pos.y), cam.pos) < RENDER_DISTANCE && chunk.created_data)
        {
            chunk.draw_transparent(*shader);
        }
    }
    load_3chunks();
    create_chunks();
    unload_3chunks();

    glfwSwapBuffers(window);
    glfwPollEvents(); 
}

void Game::save_chunk(glm::ivec2 pos)
{
    std::string path = "save/" + std::to_string(pos.x) + "," + std::to_string(pos.y) + ".chunk";
    std::vector<uint8_t> data = utill::world_data_to_uint8(chunks[pos].data);
    utill::write_file_binary(path, data.data(), data.size() * sizeof(uint8_t));
}

void Game::load_3chunks()
{
    if (there_chunks_left_to_load)
    {
        size_t loaded = 0;
        while (!to_load.empty() && loaded < 3)
        {
            auto pos = to_load.back();

            if(!chunks.count(pos))
            {
                chunks.try_emplace(pos, glm::vec3(pos.x, 0, pos.y));
            }
            
            std::string path = "save/" + std::to_string(pos.x) + "," + std::to_string(pos.y) + ".chunk";
            
            if (!utill::file_exist(path) && !chunks[pos].created_data)
            {
                chunks[pos].create_data(seed);
                save_chunk(pos);
            }
            else if (!chunks[pos].created_data)
            {
                load_chunk(pos);
            }

            glm::ivec2 right(pos.x + CHUNK_WIDTH, pos.y);
            glm::ivec2 left(pos.x - CHUNK_WIDTH,  pos.y);
            glm::ivec2 front(pos.x,               pos.y + CHUNK_DEPTH);
            glm::ivec2 back(pos.x,                pos.y - CHUNK_DEPTH);

            if (chunks.count(right)) chunks[right].dirty = true;
            if (chunks.count(left))  chunks[left].dirty  = true;
            if (chunks.count(front)) chunks[front].dirty = true;
            if (chunks.count(back))  chunks[back].dirty  = true;
            
            chunks[pos].added_to_load = false;
            
            to_load.pop_back();
            loaded++;
        }
        if (loaded == 0)
        {
            there_chunks_left_to_load = false;
        }
        else
        {
            there_chunks_left_to_create = true;
        }
    }
}

void Game::unload_3chunks()
{
    if (there_chunks_left_to_unload)
    {
        size_t unloaded = 0;
        while (!to_unload.empty() && unloaded < 3)
        {
            auto pos = to_unload.back();
            
            if(chunks.count(pos))
            {
                std::string path = "save/" + std::to_string(pos.x) + "," + std::to_string(pos.y) + ".chunk";
                if (!utill::file_exist(path))
                {
                    save_chunk(pos);
                }
                chunks.erase(pos);
                unloaded++;
            }

            to_unload.pop_back();   
        }

        if (unloaded == 0)
        {
            there_chunks_left_to_unload = false;
        }
    }
}

void Game::create_chunks()
{
    if (there_chunks_left_to_create)
    {
        size_t created = 0;
        for (auto& [pos, chunk] : chunks)
        {
            if (chunk.created_data && chunk.dirty)
            {
                chunk.build_mesh(chunks); 
                created++;
            }
        }

        if(created == 0)
        {
            there_chunks_left_to_create = false;
        }
    }
}

void Game::load_chunk(glm::ivec2 pos)
{
    std::string path = "save/" + std::to_string(pos.x) + "," + std::to_string(pos.y) + ".chunk";
    std::vector<uint8_t> data = utill::read_file_binary_vector(path);

    assert(!data.empty());

    for (size_t i = 0; i < data.size(); i++)
    {
        size_t x = i / (CHUNK_DEPTH * CHUNK_HEIGHT);
        size_t z = (i / CHUNK_HEIGHT) % CHUNK_DEPTH;
        size_t y = i % CHUNK_HEIGHT;
        
        chunks[pos].data[x][z][y] = static_cast<BlockType>(data[i]);
    }
    
    chunks[pos].created_data = true;
    chunks[pos].dirty = true;
}

void Game::unload_far_chunks()
{
    glm::vec2 play_pos = {
        cam.pos.x,
        cam.pos.z
    };
    for (auto& [pos, chunk] : chunks)
    {
        glm::vec2 chunk_pos = {
            (float)pos.x,
            (float)pos.y
        };
        if (glm::distance(play_pos, chunk_pos) > (RENDER_DISTANCE + 1) && !chunk.added_to_unload)
        {
            to_unload.push_back(pos);
            chunk.added_to_unload = true;
        }
    }
}

bool Game::change_block(int i, BlockType type)
{
    glm::vec3 pos = cam.pos + cam.direction * (float)i;

    glm::ivec2 current_chunk(utill::world_pos_to_chunk_index(pos));
    glm::ivec2 chunk_key(current_chunk.x * CHUNK_WIDTH, current_chunk.y * CHUNK_DEPTH);

    if (!chunks.count(chunk_key) || !chunks[chunk_key].created_data)
    {
        return false;
    }
            
    glm::vec3 local_pos(utill::world_pos_to_chunk_pos(pos, current_chunk));

    if (local_pos.x < 0 || local_pos.x >= CHUNK_WIDTH ||
            local_pos.y < 0 || local_pos.y >= CHUNK_HEIGHT ||
            local_pos.z < 0 || local_pos.z >= CHUNK_DEPTH)
    {
        return false;
    }

    chunks[chunk_key].data[(int)local_pos.x][(int)local_pos.z][(int)local_pos.y] = type;
    chunks[chunk_key].dirty = true;
    if ((int)local_pos.x == 0) 
    {
        glm::ivec2 key(chunk_key.x - CHUNK_WIDTH, chunk_key.y);
        if (chunks.count(key)) chunks[key].dirty = true;
    }
    if ((int)local_pos.x == CHUNK_WIDTH - 1) 
    {
        glm::ivec2 key(chunk_key.x + CHUNK_WIDTH, chunk_key.y);
        if (chunks.count(key)) chunks[key].dirty = true;
    }
    if ((int)local_pos.z == 0) 
    {
        glm::ivec2 key(chunk_key.x, chunk_key.y - CHUNK_DEPTH);
        if (chunks.count(key)) chunks[key].dirty = true;
    }
    if ((int)local_pos.z == CHUNK_DEPTH - 1) 
    {
        glm::ivec2 key(chunk_key.x, chunk_key.y + CHUNK_DEPTH);
        if (chunks.count(key)) chunks[key].dirty = true;
    }
    there_chunks_left_to_create = true;
    return true;
}

Game::~Game()
{
    delete shader;
    delete texture;
    delete skybox;
#if ZPREPASS
    delete zprepass_shader;
#endif
}

namespace utill
{
    std::vector<uint8_t> world_data_to_uint8(BlockType (&data)[CHUNK_WIDTH][CHUNK_DEPTH][CHUNK_HEIGHT])
    {
        std::vector<uint8_t> vector;
        vector.reserve(CHUNK_WIDTH * CHUNK_DEPTH * CHUNK_HEIGHT);

        for(size_t x = 0; x < CHUNK_WIDTH; x++)
        {
            for(size_t z = 0; z < CHUNK_DEPTH; z++)
            {
                for(size_t y = 0; y < CHUNK_HEIGHT; y++)
                {
                    vector.push_back(static_cast<uint8_t>(data[x][z][y]));
                }
            }
        }
        return vector;
    }

    inline glm::vec3 world_pos_to_chunk_pos(glm::vec3 pos, glm::ivec2 current_chunk)
    {
        int local_x = (int)(std::floor(pos.x)) - (current_chunk.x * CHUNK_WIDTH);
        int local_z = (int)(std::floor(pos.z)) - (current_chunk.y * CHUNK_DEPTH);
        int local_y = (int)(std::floor(pos.y));
        return glm::vec3(local_x, local_y, local_z);
    }
    inline glm::ivec2 world_pos_to_chunk_index(glm::vec3 pos)
    {
        int chunk_x = (int)std::floor(pos.x / CHUNK_WIDTH);
        int chunk_z = (int)std::floor(pos.z / CHUNK_DEPTH);
        return glm::ivec2(chunk_x, chunk_z);
    }
}