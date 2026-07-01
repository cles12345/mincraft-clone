#pragma once
#include <glm.hpp>
#include "camera.hpp"

struct Plane
{
    glm::vec3 normal = {0.0f, 1.0f, 0.0f};

    float distance = 0.0f;

    Plane() = default;

    Plane(const glm::vec3& p1, const glm::vec3& norm)
        : normal(glm::normalize(norm)),
        distance(-glm::dot(normal, p1))
        {}
};

struct AABB
{
    glm::vec3 min;
    glm::vec3 max;
};

class Frustum
{
    public:
        Plane top_face;
        Plane bottom_face;

        Plane right_face;
        Plane left_face;

        Plane far_face;
        Plane near_face;

        Frustum() = default;

        void update(Camera& cam, float z_far);
        bool is_visable(const AABB& box);
};