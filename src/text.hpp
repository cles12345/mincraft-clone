#pragma once
#include <vector>
#include <glm.hpp>
#include "vao.hpp"
#include "vbo.hpp"
#include "ebo.hpp"
#include "shader.hpp"

constexpr size_t CHAR_WIDTH = 16;
constexpr size_t CHAR_HEIGHT = 16;

struct Char
{
    char character;
    float x, y;
    float r, g, b;
};

class TextRenderer
{
    public:
        std::vector<Char> text;
        std::vector<CharVertex> vertices;
        std::vector<unsigned int> indices;
        VAO vao = VAO(sizeof(CharVertex));
        VBO vbo;
        EBO ebo;
        Shader shader;
        bool dirty;

        void render(Shader& shader);
        void build_mesh();
        void put(std::string& text);
        std::array<float, 2> get_tile(char c);
};

