#include "ebo.hpp"

EBO::EBO(const unsigned int indicies[], size_t indicies_size)
{
    indices_count = indicies_size/sizeof(unsigned int);

    glGenBuffers(1, &id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id); 
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies_size, indicies, GL_STATIC_DRAW);
}

EBO::EBO()
{
    glGenBuffers(1, &id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id); 
}

void EBO::send_buffer(const unsigned int indicies[], size_t indicies_size)
{
    indices_count = indicies_size/sizeof(unsigned int);
    bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies_size, indicies, GL_STATIC_DRAW);
}

void EBO::bind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id); 
}

EBO::~EBO()
{
    glDeleteBuffers(1, &id);
}