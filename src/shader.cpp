#include "shader.hpp"

Shader::Shader(const std::string& vertex_path, const std::string& fragment_path)
{
    vertex_shader =  glCreateShader(GL_VERTEX_SHADER);
    std::string vertex_code = utill::read_file(vertex_path);    
    const char *vertex_shader_src = vertex_code.c_str();
    glShaderSource(vertex_shader, 1, &vertex_shader_src, NULL);
    glCompileShader(vertex_shader);

    int  success = 1;
    char infoLog[512] = {0};
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    
    if(!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        std::cout << "Error vertex shader failed to compile\n" << infoLog << "\n";
        assert(false);
    }

    fragment_shader =  glCreateShader(GL_FRAGMENT_SHADER);
    std::string fragment_code = utill::read_file(fragment_path);    
    const char *fragment_shader_src = fragment_code.c_str();
    glShaderSource(fragment_shader, 1, &fragment_shader_src, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    
    if(!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
        std::cout << "Error fragment shader failed to compile\n" << infoLog << "\n";
        assert(success);
    }

    shader_program = glCreateProgram();

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);

    if(!success)
    {
        glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
        std::cout << "Error shader program failed to link\n" << infoLog << "\n";
        assert(success);
    }
}

void Shader::use() const
{
    glUseProgram(shader_program);
}

void Shader::set_uniform(float value, const char* uniform_name)
{
    unsigned int uniform_location = glGetUniformLocation(shader_program, uniform_name);
    glUseProgram(shader_program);
    glUniform1f(uniform_location, value);
}

void Shader::set_uniform(float value1, float value2, const char* uniform_name)
{
    unsigned int uniform_location = glGetUniformLocation(shader_program, uniform_name);
    glUseProgram(shader_program);
    glUniform2f(uniform_location, value1, value2);
}

void Shader::set_uniform(float value1, float value2, float value3, const char* uniform_name)
{
    unsigned int uniform_location = glGetUniformLocation(shader_program, uniform_name);
    glUseProgram(shader_program);
    glUniform3f(uniform_location, value1, value2, value3);
}

void Shader::set_uniform(float value1, float value2, float value3, float value4, const char* uniform_name)
{
    unsigned int uniform_location = glGetUniformLocation(shader_program, uniform_name);
    glUseProgram(shader_program);
    glUniform4f(uniform_location, value1, value2, value3, value4);
}

void Shader::set_uniform(const glm::mat4& value, const char* uniform_name)
{
    unsigned int uniform_location = glGetUniformLocation(shader_program, uniform_name);
    glUseProgram(shader_program);
    glUniformMatrix4fv(uniform_location, 1, GL_FALSE, glm::value_ptr(value));
}

Shader::~Shader()
{
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glDeleteProgram(shader_program);
}

