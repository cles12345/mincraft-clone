#pragma once
#include <glad.h>
#include <glfw3.h>
#include <cassert>

class Texture
{
    public:
        unsigned int texture;

        Texture() = default;
        Texture(const char* path, unsigned int wrap_s, unsigned int wrap_t, unsigned int min_filter, unsigned int mag_filter);
        void bind();
};
