#include "block.hpp"

Block::Block(BlockType type, Shader& shader) : data{Texture("sprite/grass.jpeg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR),
           {{0.1f, 0.3f, 0.1f}, {0.4f, 0.8f, 0.3f}, {0.2f, 0.2f, 0.2f}, 2.0f}}
{
    shader.use();
    shader.set_uniform(data.material, "material");
}

void Block::draw(Shader& shader, Mesh& mesh, glm::vec3 pos)
{
    glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
    shader.set_uniform(model, "model");
    shader.set_uniform(data.material, "material");
    data.texture.bind(); 
    glDrawElements(GL_TRIANGLES, mesh.indices_count, GL_UNSIGNED_INT, 0);
}
