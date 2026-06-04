#pragma once
#include "shader.hpp"
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#define BLOCK_MAX 100.0f
#define BLOCK_MIN 0.1f

class Camera
{
    public:
        glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f); 
        glm::vec3 direction = glm::vec3(0.0f, -0.5f, -1.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); 
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, BLOCK_MIN, BLOCK_MAX);
        
        float pitch = 0.0f;
        float yaw = -90.f;
        bool first_mouse = true;
        float last_x = 0;
        float last_y = 0;

        Camera(float fov, float width, float height);
        void mouse_callback(double x, double y);
        glm::mat4 get_view();
        void update(Shader& shader);
        void move_forward(float speed);
        void move_backward(float speed);
        void move_right(float speed);
        void move_left(float speed);
        void move_up(float speed);
        void move_down(float speed);
};
