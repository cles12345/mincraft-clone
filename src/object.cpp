#include "object.hpp"

Object::Object(const float *vertices, const unsigned int *indicies, size_t vertex_size, size_t indicies_size, std::string vertex_shader_path, std::string fragment_shader_path) : 
     mesh(vertices, vertex_size, indicies, indicies_size),
     shader(vertex_shader_path, fragment_shader_path),
     indices((unsigned int*)indicies) {}

Object::Object(const float *vertices, const unsigned int *indicies, size_t vertex_size, size_t indicies_size, Shader& shader) : 
     mesh(vertices, vertex_size, indicies, indicies_size),
     shader(shader), 
     indices((unsigned int*)indicies) {}

void Object::draw()
{
    shader.use();
    mesh.bind();
    glDrawElements(GL_TRIANGLES, mesh.indices_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
