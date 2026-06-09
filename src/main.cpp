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
        std::string seed_str = utill::read_file("save/seed");
        seed = stoi(seed_str);
    }
    else
    {
        seed = rand();
        utill::write_file("save/seed", std::to_string(seed));
    }

    shader = new Shader("shaders/shader.vert", "shaders/shader.frag");
    texture = new Texture("sprite/atlas.png", GL_REPEAT, GL_REPEAT, GL_NEAREST_MIPMAP_LINEAR, GL_NEAREST);
    last_frame = glfwGetTime();

    cam.pos = glm::vec3(0.0f, 200.0f, 0.0f);

    last_chunk.x = std::floor(cam.pos.x / CHUNK_WIDTH);
    last_chunk.y = std::floor(cam.pos.z / CHUNK_DEPTH);

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
        if (current_chunk  != last_chunk)
        {
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
    float light_pos[3] = {0.0f, 270.0f, -3.0f};

    shader->set_uniform(light_color[0], light_color[1], light_color[2], "lightColor");
    shader->set_uniform(light_pos[0], light_pos[1], light_pos[2], "lightPos");
    shader->set_uniform(cam.pos[0], cam.pos[1], cam.pos[2], "viewPos");

    float fps = 1.0f / delta_time;

    std::string title = "FPS: "; 
    title += std::to_string(fps);
    glfwSetWindowTitle(window, title.c_str());

    to_load.clear();
    to_unload.clear();

    int i = 0;
    for (auto& [pos, chunk] : chunks)
    {
        if (i >= 3) break; 

        if (!chunk.created_data || chunk.dirty)
        {
            to_load.push_back(pos);
        }
    }

    for (auto& pos : to_load)
    {
        if(chunks.count(pos))
        {
            std::string path = "save/";
            path += std::to_string(pos.x);
            path += ",";
            path += std::to_string(pos.y);
            path += ".chunk";
            if (!utill::file_exist(path))
            {
                chunks[pos].create_data(seed);
                save_chunk(pos);
            }
            else
            {
                load_chunk(pos);
            }
            chunks[pos].build_mesh(chunks);
        }
        to_load.pop_back();
    }

    for (auto& pos : to_unload)
    {
        if(chunks.count(pos))
        {
            std::string path = "save/";
            path += std::to_string(pos.x);
            path += ",";
            path += std::to_string(pos.y);
            path += ".chunk";
            if (!utill::file_exist(path))
            {
                save_chunk(pos);
            }
            chunks.erase(pos);
        }
        to_unload.pop_back();
    }
    
    for (auto& [pos, chunk] : chunks)
    {

        if (glm::distance(glm::vec3(pos.x, cam.pos.y, pos.y), cam.pos) < RENDER_DISTANCE && !chunk.dirty &&  chunk.created_data)
        {
            chunk.draw(*shader);
        }
    }

    glfwSwapBuffers(window);
    glfwPollEvents();    
}

void Game::save_chunk(glm::ivec2 pos)
{
    std::string path = "save/";
    path += std::to_string(pos.x);
    path += ",";
    path += std::to_string(pos.y);
    path += ".chunk";

    utill::write_file(path, utill::world_data_to_string(chunks[pos].data));
}

void Game::load_chunk(glm::ivec2 pos)
{
    std::string path = "save/";
    path += std::to_string(pos.x);
    path += ",";
    path += std::to_string(pos.y);
    path += ".chunk";
    std::string data = utill::read_file(path);

    assert(!data.empty());

    size_t i = 0;
    for (size_t x = 0; x < CHUNK_WIDTH; x++)
    {
        for (size_t z = 0; z < CHUNK_DEPTH; z++)
        {
            for (size_t y = 0; y < CHUNK_HEIGHT; y++)
            {  
                while (i < data.size() && data[i] == ' ') 
                {
                    i++;
                }
                
                if (i >= data.size()) break;
                
                char c = data[i];
                switch(c)
                {
                    case '0': chunks[pos].data[x][z][y] = NONE; break;
                    case '1': chunks[pos].data[x][z][y] = GRASS_TYPE; break;
                    case '2': chunks[pos].data[x][z][y] = DIRT_TYPE; break;
                    case '3': chunks[pos].data[x][z][y] = STONE_TYPE; break;
                    case '4': chunks[pos].data[x][z][y] = WATER_TYPE; break;
                }
                i++;
            }
        }
    }
    chunks[pos].created_data = true;
}

void Game::unload_far_chunks()
{
     glm::ivec2 player_chunk = {
        (int)std::floor(cam.pos.x / CHUNK_WIDTH),
        (int)std::floor(cam.pos.z / CHUNK_DEPTH)
    };
    
    for (auto it = chunks.begin(); it != chunks.end();)
    {
        int chunk_x = it->first.x / CHUNK_WIDTH;
        int chunk_z = it->first.y / CHUNK_DEPTH;
        
        int dx = abs(chunk_x - player_chunk.x);
        int dz = abs(chunk_z - player_chunk.y);
        
        if (dx > RENDER_DISTANCE_CHUNKS + 1 || dz > RENDER_DISTANCE_CHUNKS + 1)
        {
            save_chunk(it->first);
            it = chunks.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

namespace utill
{
    std::string world_data_to_string(BlockType (&data)[CHUNK_WIDTH][CHUNK_DEPTH][CHUNK_HEIGHT])
    {
        std::string str = "";

        for(size_t x = 0; x < CHUNK_WIDTH; x++)
        {
            for(size_t z = 0; z < CHUNK_DEPTH; z++)
            {
                for(size_t y = 0; y < CHUNK_HEIGHT; y++)
                {
                    switch (data[x][z][y])
                    {
                        case NONE: str += "0 "; break;
                        case GRASS_TYPE: str += "1 "; break;
                        case DIRT_TYPE: str += "2 "; break;
                        case STONE_TYPE: str += "3 "; break;
                        case WATER_TYPE: str += "4 "; break;
                    }
                }
            }
        }
        return str;
    }
}