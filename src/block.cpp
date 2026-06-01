#include "block.hpp"

Block::Block(BlockType type) : data{get_block_data(type)}
{}

void Block::draw(Shader& shader, Mesh& mesh, glm::vec3 pos)
{
    glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
    shader.set_uniform(model, "model");
    shader.set_uniform(data.material, "material");
    data.texture.bind(); 
    glDrawElements(GL_TRIANGLES, mesh.indices_count, GL_UNSIGNED_INT, 0);
}

BlockData get_block_data(BlockType type)
{
    switch (type)
    {
    case GRASS_TYPE: return {Texture("sprite/grass.jpeg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR), {{0.1f, 0.3f, 0.1f}, {0.4f, 0.8f, 0.3f}, {0.2f, 0.2f, 0.2f}, 2.0f}};
    case STONE_TYPE: return {Texture("sprite/stone.jpeg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR), {{0.2f, 0.2f, 0.2f}, {0.5f, 0.5f, 0.5f}, {0.1f, 0.1f, 0.1f}, 2.0f}};
    default: assert(false && "unknown block_type");
    }
}