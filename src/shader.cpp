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
        assert(false);
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
        assert(false);
    }
}

void Shader::use() const
{
    glUseProgram(shader_program);
}

void Shader::set_uniform_4f(float value[4], const char* uniform_name)
{
    unsigned int uniform_location = glGetUniformLocation(shader_program, uniform_name);
    use();  
    glUniform4f(uniform_location, value[0], value[1], value[2], value[3]);
}

Shader::~Shader()
{
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glDeleteProgram(shader_program);
}

