#include "vao.hpp"

VAO::VAO(unsigned int stride)
{
    this->stride = stride;
    glGenVertexArrays(1, &id);
}

VAO::VAO(VAO&& other) noexcept 
    : id(other.id), count(other.count), stride(other.stride)
{
    other.id = 0;
}

VAO& VAO::operator=(VAO&& other) noexcept
{
    if (this != &other)
    {
        glDeleteVertexArrays(1, &id);
        id = other.id;
        count = other.count;
        stride = other.stride;
        other.id = 0;
    }
    return *this;
}

void VAO::bind()
{
    glBindVertexArray(id);
}

void VAO::set_layout(unsigned int location, int elements, VERTEX_TYPE type)
{
    unsigned int element_size = 0;
    switch (type)
    {
        case FLOAT:
            element_size = sizeof(float);
            glVertexAttribPointer(location, elements, GL_FLOAT, GL_FALSE, stride, (void*)(uintptr_t)count);
            glEnableVertexAttribArray(location); 
            break;
        case UNSIGND_INT:
            element_size = sizeof(unsigned int);
            glVertexAttribIPointer(location, elements, GL_UNSIGNED_INT, stride, (void*)(uintptr_t)count);
            glEnableVertexAttribArray(location); 
            break;
        case INT:
            element_size = sizeof(int);
            glVertexAttribIPointer(location, elements, GL_INT, stride, (void*)(uintptr_t)count);
            glEnableVertexAttribArray(location); 
            break;
    }
    count += elements * element_size;
}

VAO::~VAO()
{
    glDeleteVertexArrays(1, &id);
}