#include "vbo.hpp"

VBO::VBO(const float vertices[], size_t vertex_size)
{
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, vertex_size, vertices, GL_STATIC_DRAW);
}

VBO::VBO()
{
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

VBO::VBO(VBO&& other) noexcept : id(other.id)
{
    other.id = 0;
}

VBO& VBO::operator=(VBO&& other) noexcept
{
    if (this != &other)
    {
        glDeleteBuffers(1, &id);
        id = other.id;
        other.id = 0;
    }
    return *this;
}

void VBO::send_buffer(const Vertex vertices[], unsigned int elements)
{
    bind();
    glBufferData(GL_ARRAY_BUFFER, elements * sizeof(Vertex), vertices, GL_STATIC_DRAW);
}

void VBO::send_buffer(const CharVertex vertices[], unsigned int elements)
{
    bind();
    glBufferData(GL_ARRAY_BUFFER, elements * sizeof(CharVertex), vertices, GL_DYNAMIC_DRAW);
}

void VBO::send_buffer(const float vertices[], size_t size)
{
    bind();
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

void VBO::bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

VBO::~VBO()
{
    glDeleteBuffers(1, &id);
}