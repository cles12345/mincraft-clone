#pragma once 
#include <glad.h>
#include <glfw3.h>

enum VERTEX_TYPE 
{
    FLOAT,
    INT,
    UNSIGND_INT
};

class VAO
{
    public:
        unsigned int id = 0, count = 0, stride = 0;
        
        VAO(unsigned int stride);
        void bind();    
        void set_layout(unsigned int location, int elements, VERTEX_TYPE type);
        ~VAO();
};