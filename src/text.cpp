#include "text.hpp"

void TextRenderer::render(Shader& shader)
{
    if (vertices.empty()) return;
    vao.bind();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    glDisable(GL_DEPTH_TEST);
    shader.set_uniform(projection, "projection");
    glDrawElements(GL_TRIANGLES, ebo.indices_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void TextRenderer::build_mesh()
{
    vertices.clear();
    indices.clear();
    
    vao.count = 0;
    for (auto& c : text)
    {
        auto uv = get_tile(c.character);
        float u0 = uv[0];
        float v0 = uv[1];
        float u1 = u0 + (1.0f / CHAR_WIDTH);
        float v1 = v0 + (1.0f / CHAR_HEIGHT);

        float x = c.x * CHAR_WIDTH;
        float y = c.y * CHAR_HEIGHT;
        vertices.emplace_back(x, y, c.r, c.g, c.b, u0, v0);
        vertices.emplace_back(x + CHAR_WIDTH, y, c.r, c.g, c.b, u1, v0);
        vertices.emplace_back(x + CHAR_WIDTH, y + CHAR_HEIGHT, c.r, c.g, c.b, u1, v1);
        vertices.emplace_back(x, y + CHAR_HEIGHT, c.r, c.g, c.b, u0, v1);

        unsigned int offset = vertices.size() - 4;
        indices.emplace_back(offset + 0);
        indices.emplace_back(offset + 1);
        indices.emplace_back(offset + 2);
        indices.emplace_back(offset + 0);
        indices.emplace_back(offset + 2);
        indices.emplace_back(offset + 3);
    }
    
    vao.bind();
    vbo.bind();
    ebo.bind();
    vao.set_layout(0, 2, VERTEX_TYPE::FLOAT);
    vao.set_layout(1, 3, VERTEX_TYPE::FLOAT);
    vao.set_layout(2, 2, VERTEX_TYPE::FLOAT);
    vbo.send_buffer(vertices.data(), vertices.size());
    ebo.send_buffer(indices.data(), indices.size() * sizeof(unsigned int));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    dirty = false;
}

void TextRenderer::put(std::string& str)
{
    text.clear();
    size_t x = 0;
    size_t y = 0;
    size_t max_cols = 800 / CHAR_WIDTH;
    size_t max_rows = 600 / CHAR_HEIGHT;
    for (char c : str)
    {
        Char ch = { };
        ch.character = c;
        ch.x = (float)x;
        ch.y = (float)y;
        ch.r = 1.0f;
        ch.g = 1.0f;
        ch.b = 1.0f;
        text.push_back(ch);
        x++;
        if (x >= max_cols)
        {
            x = 0;
            y++;
        }
        
        if (y >= max_rows)
            break;
    }
    dirty = true;
}

std::array<float, 2> TextRenderer::get_tile(char c)
{
    int tile_index = 0;

    switch (c)
    {
    case 'F': tile_index = 0; break;
    case 'P': tile_index = 1; break;
    case 'S': tile_index = 2; break;
    case '.': tile_index = 3; break;
    case '1': tile_index = 4; break;
    case '2': tile_index = 5; break;
    case '3': tile_index = 6; break;
    case '4': tile_index = 7; break;
    case '5': tile_index = 8; break;
    case '6': tile_index = 9; break;
    case '7': tile_index = 10; break;
    case '8': tile_index = 11; break;
    case '9': tile_index = 12; break;
    case '0': tile_index = 13; break;
    case ':': tile_index = 14; break;
    case ' ': tile_index = 15; break;
    default: assert(false && "invalid char to get the tile");
    }

    int col = tile_index % CHAR_HEIGHT;
    int row = tile_index / CHAR_HEIGHT;
    float u = col * (1.0f / CHAR_HEIGHT);
    float v = 1.0f - ((row + 1) * (1.0f / CHAR_WIDTH));

    return { u, v };
}
