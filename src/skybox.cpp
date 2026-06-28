#include "skybox.hpp"

Skybox::Skybox()
{
    // Vertices (24 unique vertices)
    float vertices[] = {
        // positions
        // Right (+X)
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        // Left (-X)
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        // Top (+Y)
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        // Bottom (-Y)
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        // Front (+Z)
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        // Back (-Z)
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
    };

    // Indices (36 triangles)
    unsigned int indices[] = {
        // Right
        0, 1, 2,
        1, 3, 2,
        // Left
        4, 5, 6,
        5, 7, 6,
        // Top
        8, 9, 10,
        9, 11, 10,
        // Bottom
        12, 13, 14,
        13, 15, 14,
        // Front
        16, 17, 18,
        17, 19, 18,
        // Back
        20, 21, 22,
        21, 23, 22,
    };
    vao.bind();
    vbo.bind();
    vbo.send_buffer(vertices, sizeof(vertices));
    ebo.bind();
    ebo.send_buffer(indices, sizeof(indices));
    vao.set_layout(0, 3, VERTEX_TYPE::FLOAT);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    std::string paths[6] =
    {
        "sprite/px.png",
        "sprite/nx.png",
        "sprite/py.png",
        "sprite/ny.png",
        "sprite/pz.png",
        "sprite/nz.png"
    };

    for (size_t i = 0; i < 6; i++)
    {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(false);
        unsigned char* data = stbi_load(paths[i].c_str(), &width, &height, &channels, 0);
        assert(data && "Couldn't load sprite");

        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB; 
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }
        
    shader.use();
    shader.set_uniform(0, "skybox");
}

void Skybox::draw(Camera& camera)
{
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);

    shader.use();

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    view = glm::mat4(glm::mat3(glm::lookAt(camera.pos, camera.pos + camera.direction, camera.up)));
    projection = glm::perspective(glm::radians(camera.fov), camera.width / camera.height, BLOCK_MIN, BLOCK_MAX);
    shader.set_uniform(view, "view");
    shader.set_uniform(projection, "projection");

    vao.bind();
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glDrawElements(GL_TRIANGLES, ebo.indices_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}

Skybox::~Skybox()
{
    glDeleteTextures(1, &texture);
}