#pragma once
#include "shader.hpp"
#include "utill.hpp"
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

constexpr float BLOCK_MAX = 100000.0f;
constexpr float BLOCK_MIN = 0.1f;

namespace utill
{
    inline bool is_look_left(glm::vec3 direction) {return direction.x < 0.0f;};
    inline bool is_look_right(glm::vec3 direction) {return direction.x > 0.0f;};
    inline bool is_look_up(glm::vec3 direction) {return direction.y > 0.0f;};
    inline bool is_look_down(glm::vec3 direction) {return direction.y < 0.0f;};
    inline bool is_look_front(glm::vec3 direction) {return direction.z > 0.0f;};
    inline bool is_look_back(glm::vec3 direction) {return direction.z < 0.0f;};
};

class Camera
{
    public:
        glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f); 
        glm::vec3 direction = glm::vec3(0.0f, -0.5f, -1.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); 
        glm::mat4 view = glm::mat4(1.0f);
        float fov = 45.0f;
        float width = 800.0f;
        float height = 600.f;
        glm::mat4 projection = glm::perspective(glm::radians(fov), width / height, BLOCK_MIN, BLOCK_MAX);
        
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
