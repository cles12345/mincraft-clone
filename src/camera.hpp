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
        glm::vec3 pos;  
        glm::vec3 direction;
        glm::vec3 up; 
        glm::mat4 view;
        glm::mat4 projection;
        
        float pitch;
        float yaw;
        bool first_mouse;
        float last_x;
        float last_y;
        
        Camera();
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
