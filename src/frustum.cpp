#include "frustum.hpp"

void Frustum::update(Camera& cam, float z_far)
{
    const float half_v_side = z_far * tanf(glm::radians(cam.fov * 0.5f));
    const float half_h_side = half_v_side * (cam.width / cam.height);
    const glm::vec3 front_mult_far = z_far * cam.direction;

    near_face = {cam.pos + BLOCK_MIN * cam.direction, cam.direction};
    far_face  = {cam.pos + front_mult_far, -cam.direction};
    right_face = {cam.pos, glm::cross(front_mult_far - cam.right * half_h_side, cam.up)};

    left_face = {cam.pos, glm::cross(cam.up, front_mult_far + cam.right * half_h_side)};

    top_face = {cam.pos, glm::cross(cam.right, front_mult_far - cam.up * half_v_side)};

    bottom_face = {cam.pos, glm::cross(front_mult_far + cam.up * half_v_side, cam.right)};
}

bool Frustum::is_visable(const AABB& box)
{
    Plane* planes[6];
    planes[0] = &near_face;
    planes[1] = &far_face;
    planes[2] = &right_face;
    planes[3] = &left_face;
    planes[4] = &top_face;
    planes[5] = &bottom_face;
    for (size_t i = 0; i < 6; i++)
    {
        glm::vec3 point = box.min;

        if (planes[i]->normal.x >= 0) point.x = box.max.x;
        if (planes[i]->normal.y >= 0) point.y = box.max.y;
        if (planes[i]->normal.z >= 0) point.z = box.max.z;

        float distance = glm::dot(planes[i]->normal, point) + planes[i]->distance;
        if (distance < 0.0) return false;
    }
    return true;
}