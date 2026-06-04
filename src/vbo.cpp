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

void VBO::send_buffer(const Vertex vertices[], unsigned int elements)
{
    glBufferData(GL_ARRAY_BUFFER, elements * sizeof(Vertex), vertices, GL_STATIC_DRAW);
}

void VBO::bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

VBO::~VBO()
{
    glDeleteBuffers(1, &id);
}