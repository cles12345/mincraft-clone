#include "camera.hpp"

Camera::Camera(float fov, float width, float height)
{
    this->fov = fov;
    this->width = width;
    this->height = height;
    projection = glm::perspective(glm::radians(fov), width / height, BLOCK_MIN, BLOCK_MAX);
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
    shader.use();
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