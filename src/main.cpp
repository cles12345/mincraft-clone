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
    
    shader = new Shader("shaders/shader.vert", "shaders/shader.frag");
    chunk = new Chunk();
    texture = new Texture("sprite/atlas.png", GL_REPEAT, GL_REPEAT, GL_NEAREST_MIPMAP_LINEAR, GL_NEAREST);
    last_frame = glfwGetTime();

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

    float light_color[3] = {1.0f, 1.0f, 1.0f};
    float light_pos[3] = {0.0f, 20.0f, -3.0f};

    shader->set_uniform(light_color[0], light_color[1], light_color[2], "lightColor");
    shader->set_uniform(light_pos[0], light_pos[1], light_pos[2], "lightPos");
    shader->set_uniform(cam.pos[0], cam.pos[1], cam.pos[2], "viewPos");
    
    std::cout << "FPS: " << 1.0f / delta_time << "\n";

    chunk->draw(*shader);

    glfwSwapBuffers(window);
    glfwPollEvents();    
}