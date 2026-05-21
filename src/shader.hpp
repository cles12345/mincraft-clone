#pragma once
#include <glad.h>
#include <glfw3.h>
#include "utill.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>

class Shader
{
    public:
        unsigned int vertex_shader;
        unsigned int fragment_shader;
        unsigned int shader_program;
        
        Shader(const std::string& vertex_path, const std::string& fragment_path);
        void use() const;
        void set_uniform_4f(float value[4], const char* uniform_name);
        ~Shader();
};