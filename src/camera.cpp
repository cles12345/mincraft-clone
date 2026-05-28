#include "camera.hpp"

Camera::Camera() : yaw(-90.0f), pitch(0.0f), first_mouse(true), view(1.0f)
{
    pos= glm::vec3(0.0f, 0.0f, 3.0f);  
    direction = glm::vec3(0, 0, -1);
    up = glm::vec3(0.0f, 1.0f, 0.0f);
    yaw = -90.0f;
    pitch = 0.0f;
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
}

void Camera::mouse_callback(double x, double y)
{
    if (first_mouse)
    {
        last_x = x;
        last_y = y;
        first_mouse = false;
    }
  
    float xoffset = x - last_x;
    float yoffset = last_y - y; 
    last_x = x;
    last_y = y;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction = glm::normalize(direction);
}  

glm::mat4 Camera::get_view() 
{
    return glm::lookAt(pos, pos + direction, up);
}

void Camera::update(Shader& shader)
{
    view = get_view();
    shader.set_uniform(view, "view");
    shader.set_uniform(projection, "projection");
}

void Camera::move_forward(float speed)
{
    pos += speed * direction;
}

void Camera::move_backward(float speed)
{
    pos -= speed * direction;
}

void Camera::move_right(float speed)
{
    pos += glm::normalize(glm::cross(direction, up)) * speed;
}

void Camera::move_left(float speed)
{
    pos -= glm::normalize(glm::cross(direction, up)) * speed;
}

void Camera::move_up(float speed)
{
    pos += speed * up;
}

void Camera::move_down(float speed)
{
    pos -= speed * up;
}