#include <iostream>
#include <glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#include <cmath>
#include "shader.hpp"
#include "mesh.hpp"
#include "object.hpp"
#include "texture.hpp"
#include "camera.hpp"
#include "block.hpp"

void check_events(Camera& cam, GLFWwindow* window, float delta_time);

int main(int argc, char const *argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    
    GLFWwindow* window = glfwCreateWindow(800, 600, "game", NULL, NULL);
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
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
        0.5f,  0.5f,  0.5f, 0.0f, 1.0f,

        // Back face
        0.5f,  0.5f, -0.5f,   0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,   0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,

        // Left face
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,

        // Right face
        0.5f,  0.5f,  0.5f,   0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
        0.5f,  0.5f, -0.5f,    0.0f, 1.0f,

        // Top face
        -0.5f,  0.5f, -0.5f,  0.66f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.66f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.66f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.66f, 1.0f,

        // Bottom face
        -0.5f, -0.5f,  0.5f,   0.70f, 1.0f,
        -0.5f, -0.5f, -0.5f,   0.70f, 0.0f,
        0.5f, -0.5f, -0.5f,    1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,    1.0f, 1.0f
    };

    unsigned int indicies[] = {
        0,  1,  2,  0,  2,  3,   // Front
        4,  5,  6,  4,  6,  7,   // Back
        8,  9,  10, 8,  10, 11,  // Left
        12, 13, 14, 12, 14, 15,  // Right
        16, 17, 18, 16, 18, 19,  // Top
        20, 21, 22, 20, 22, 23   // Bottom
    };

    Mesh mesh(vertices, sizeof(vertices), indicies, sizeof(indicies), 5, 20);
    Shader shader("shaders/shader.vert", "shaders/shader.frag");

    mesh.set_layout(0, 3, FLOAT);
    mesh.set_layout(1, 2, FLOAT);

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    
    Block block("sprite/grass.jpeg");
    glm::vec3 pos(0.0f, 0.0f, -3.0f);
    Camera cam;
    glEnable(GL_DEPTH_TEST);
    float delta_time = 0.0f;
    float last_frame = glfwGetTime();
    while(!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        double cam_x, cam_y;
        glfwGetCursorPos(window, &cam_x, &cam_y);
        cam.mouse_callback(cam_x, cam_y);

        check_events(cam, window, delta_time);
        shader.use();
        view = cam.get_view();
        shader.set_uniform(view, "view");
        shader.set_uniform(projection, "projection");

        block.draw(shader, mesh, pos);

        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    glfwTerminate();
    return 0;
}

void check_events(Camera& cam, GLFWwindow* window, float delta_time)
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
