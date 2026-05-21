#include "mesh.hpp"

Mesh::Mesh(const float vertices[], size_t vertex_size, const unsigned int indicies[], size_t indicies_size, unsigned int elements, unsigned int stride) : vertices_count(vertex_size/sizeof(float)/elements), 
    indices_count(indicies_size/sizeof(unsigned int)), count(0), stride(stride)
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertex_size, vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); 
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies_size, indicies, GL_STATIC_DRAW);
}

void Mesh::bind()
{
    glBindVertexArray(VAO);
}

void Mesh::set_layout(unsigned int location, int elements, VERTEX_TYPE type)
{
    glBindVertexArray(VAO);
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

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

