#pragma once
#include <glad.h>
#include <glfw3.h>

class EBO
{
    public:
        unsigned int id = 0, indices_count = 0;

        EBO(const unsigned int indicies[], size_t indicies_size);
        EBO();
        void send_buffer(const unsigned int indicies[], size_t indicies_size);
        void bind();
        ~EBO();
};
