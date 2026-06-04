#include "vao.hpp"

VAO::VAO(unsigned int stride)
{
    this->stride = stride;
    
    glGenVertexArrays(1, &id);
    glBindVertexArray(id);
    glBindVertexArray(0);
}

void VAO::bind()
{
    glBindVertexArray(id);
}

void VAO::set_layout(unsigned int location, int elements, VERTEX_TYPE type)
{
    glBindVertexArray(id);
    switch (type)
    {
        case FLOAT:
            glVertexAttribPointer(location, elements, GL_FLOAT, GL_FALSE, stride, (void*)(count * sizeof(float)));
            glEnableVertexAttribArray(location); 
            break;
        case UNSIGND_INT:
            glVertexAttribPointer(location, elements, GL_UNSIGNED_INT, GL_FALSE, stride, (void*)(count * sizeof(unsigned int)));
            glEnableVertexAttribArray(location); 
            break;
        case INT:
            glVertexAttribPointer(location, elements, GL_INT, GL_FALSE, stride, (void*)(count * sizeof(int)));
            glEnableVertexAttribArray(location); 
            break;
    }
    count += elements;
    glBindVertexArray(0);  
}

VAO::~VAO()
{
    glDeleteVertexArrays(1, &id);
}