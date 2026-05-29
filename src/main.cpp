#include "main.hpp"

int main(int argc, char const *argv[])
{
    Game game;
    game.game_loop();
    return 0;
}

Game::Game() : mesh(), shader()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

    float vertices[] = {
        // Front face
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f,   1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f,   0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.333f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.333f, 1.0f,

        // Back face 
        0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f,   1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f,   0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.333f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.333f, 1.0f,

        // Left face 
        -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f,  0.0f,   1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,  0.0f,   0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f,  0.333f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,  0.333f, 1.0f,

        // Right face
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f,   1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f,   0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.333f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.333f, 1.0f,

        // Top face
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.333f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.333f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.666f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.666f, 1.0f,

        // Bottom face
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,  0.666f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  0.666f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  1.0f,   0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,  1.0f,   1.0f,
    };
    unsigned int indices[] = {
        0,  1,  2,  0,  2,  3,   // Front
        4,  5,  6,  4,  6,  7,   // Back
        8,  9,  10, 8,  10, 11,  // Left
        12, 13, 14, 12, 14, 15,  // Right
        16, 17, 18, 16, 18, 19,  // Top
        20, 21, 22, 20, 22, 23   // Bottom
    };

    mesh = new Mesh(vertices, sizeof(vertices), indices, sizeof(indices), 8, 32);
    shader = new Shader("shaders/shader.vert", "shaders/shader.frag");
    mesh->set_layout(0, 3, FLOAT);
    mesh->set_layout(1, 3, FLOAT);
    mesh->set_layout(2, 2, FLOAT);   
    
    float zvertices[] = {
        // Front face
        -0.5f,  0.5f,  0.5f, 
        -0.5f, -0.5f,  0.5f, 
        0.5f, -0.5f,  0.5f,  
        0.5f,  0.5f,  0.5f,  

        // Back face 
        0.5f,  0.5f, -0.5f,  
        0.5f, -0.5f, -0.5f,  
        -0.5f, -0.5f, -0.5f, 
        -0.5f,  0.5f, -0.5f,  

        // Left face 
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f,  0.5f, 
        -0.5f,  0.5f,  0.5f, 

        // Right face
        0.5f,  0.5f,  0.5f,  
        0.5f, -0.5f,  0.5f,  
        0.5f, -0.5f, -0.5f,  
        0.5f,  0.5f, -0.5f,  

        // Top face
        -0.5f,  0.5f, -0.5f, 
        -0.5f,  0.5f,  0.5f, 
        0.5f,  0.5f,  0.5f,  
        0.5f,  0.5f, -0.5f,  

        // Bottom face
        -0.5f, -0.5f,  0.5f, 
        -0.5f, -0.5f, -0.5f, 
        0.5f, -0.5f, -0.5f,  
        0.5f, -0.5f,  0.5f,  
    };
    zshader = new Shader("shaders/zshader.vert", "shaders/zshader.frag");
    zmesh = new Mesh(zvertices, sizeof(zvertices), indices, sizeof(indices), 3, 12);
    zmesh->set_layout(0, 3, FLOAT);

    last_frame = glfwGetTime();

    glEnable(GL_DEPTH_TEST);

    chunks.emplace_back();

    for (auto& chunk : chunks)
    {
        for (int x = 0; x < CHUNK_WIDTH; x++)
        {
            for (int y = 0; y < CHUNK_HEIGHT; y++)
            {
                for (int z = 0; z < CHUNK_DEPTH; z++)
                {
                    if (chunk.data[z + y * CHUNK_DEPTH + x * CHUNK_DEPTH * CHUNK_HEIGHT] != 0)
                    {
                        blocks.emplace_back(chunk.data[z + y * CHUNK_DEPTH + x * CHUNK_DEPTH * CHUNK_HEIGHT], *shader);
                        block_pos.emplace_back(x, y, z);
                    }
                }
            }
        }
    }
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
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cam.move_forward(3.0f * delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cam.move_backward(3.0f * delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cam.move_right(3.0f * delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cam.move_left(3.0f * delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        cam.move_up(3.0f * delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        cam.move_down(3.0f * delta_time);
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
    cam.update(*zshader);   

    float light_color[3] = {1.0f, 1.0f, 1.0f};
    float light_pos[3] = {10.0f, 20.0f, -10.0f};

    shader->set_uniform(light_color[0], light_color[1], light_color[2], "lightColor");
    shader->set_uniform(light_pos[0], light_pos[1], light_pos[2], "lightPos");
    shader->set_uniform(cam.pos[0], cam.pos[1], cam.pos[2], "viewPos");
    if (ZPREPASS)
    {
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        glColorMask(0, 0, 0, 0);
        zshader->use();
        zmesh->bind();
        for (int i = 0; i < blocks.size(); i++)
        {
            if (glm::distance(block_pos[i], cam.pos) < 100.0f)
            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), block_pos[i]);
                zshader->set_uniform(model, "model");
                glDrawElements(GL_TRIANGLES, zmesh->indices_count, GL_UNSIGNED_INT, 0);
            }
        }

        glDepthMask(GL_FALSE);
        glDepthFunc(GL_EQUAL);
        glColorMask(1, 1, 1, 1);
        shader->use();
        mesh->bind();
        for (int i = 0; i < blocks.size(); i++)
        {
            if (glm::distance(block_pos[i], cam.pos) < 100.0f)
            {
                blocks[i].draw(*shader, *mesh, block_pos[i]);
            }
        }

        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
    }
    else
    {
        glDepthFunc(GL_LESS);
        glColorMask(1, 1, 1, 1);
        shader->use();
        mesh->bind();
        for (int i = 0; i < blocks.size(); i++)
        {
            if (glm::distance(block_pos[i], cam.pos) < 100.0f)
            {
                blocks[i].draw(*shader, *mesh, block_pos[i]);
            }
        }   
    }
    std::cout << "FPS: " << 1.0f / delta_time << "\n";
    
    glfwSwapBuffers(window);
    glfwPollEvents();    
}

Game::~Game()
{
    delete shader;
    delete mesh;
    delete zshader;
    delete zmesh;
}