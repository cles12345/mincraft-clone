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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, false);

    window = glfwCreateWindow(800.0f, 600.0f, "game", NULL, NULL);

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
    texture = new Texture("sprite/atlas.png", GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST);
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
    glm::vec3 pos = cam.pos;
    for (size_t i = 0; i <= 4; i++)
    {
        pos += cam.direction;

        glm::ivec2 current_chunk(utill::world_pos_to_chunk_index(pos));
        glm::ivec2 chunk_key(current_chunk.x * CHUNK_WIDTH, current_chunk.y * CHUNK_DEPTH);

        if (!chunks.count(chunk_key) || !chunks[chunk_key].created_data)
        {
            continue;
        }
        
        glm::vec3 local_pos(utill::world_pos_to_chunk_pos(pos, current_chunk));

        if (local_pos.x < 0 || local_pos.x >= CHUNK_WIDTH ||
            local_pos.y < 0 || local_pos.y >= CHUNK_HEIGHT ||
            local_pos.z < 0 || local_pos.z >= CHUNK_DEPTH)
        {
            continue;
        }

        if(chunks[chunk_key].data[(int)local_pos.x][(int)local_pos.z][(int)local_pos.y] != BlockType::NONE)
        {
            chunks[chunk_key].data[(int)local_pos.x][(int)local_pos.z][(int)local_pos.y] = BlockType::NONE;
            chunks[chunk_key].dirty = true;
            there_chunks_left_to_create = true;
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
    shader->use();
    cam.update(*shader);

    float light_color[3] = {1.0f, 1.0f, 1.0f};
    float light_pos[3] = {cam.pos.x, cam.pos.y, cam.pos.z};

    shader->set_uniform(light_color[0], light_color[1], light_color[2], "lightColor");
    shader->set_uniform(light_pos[0], light_pos[1], light_pos[2], "lightPos");
    shader->set_uniform(cam.pos[0], cam.pos[1], cam.pos[2], "viewPos");

    float fps = 1.0f / delta_time;
    std::string title = "FPS: " + std::to_string(fps);
    glfwSetWindowTitle(window, title.c_str());

    for (auto& [pos, chunk] : chunks)
    {
        if (!chunk.created_data && !chunk.added_to_load)
        {
            to_load.push_back(pos);
            chunk.added_to_load = true;
        }

        if (glm::distance(glm::vec3(pos.x, cam.pos.y, pos.y), cam.pos) < RENDER_DISTANCE && !chunk.dirty && chunk.created_data)
        {
            chunk.draw(*shader);
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
        return glm::ivec3(local_x, local_y, local_z);
    }
    inline glm::ivec2 world_pos_to_chunk_index(glm::vec3 pos)
    {
        int chunk_x = (int)std::floor(pos.x / CHUNK_WIDTH);
        int chunk_z = (int)std::floor(pos.z / CHUNK_DEPTH);
        return glm::ivec2(chunk_x, chunk_z);
    }
}