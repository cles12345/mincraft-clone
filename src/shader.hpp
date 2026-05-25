#pragma once
#include <glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
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
        void set_uniform(float value1, float value2, const char* uniform_name);
        void set_uniform(float value1, float value2, float value3, const char* uniform_name);
        void set_uniform(float value1, float value2, float value3, float value4, const char* uniform_name);
        void set_uniform(const glm::mat4& value, const char* uniform_name);
        ~Shader();
};