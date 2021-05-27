#include "Game.h"

#include <iostream>

#include "Input.h"
#include "graphics/gl/Glew.h"
#include "graphics/gl/Shader.h"
#include "graphics/gl/VertexBuffer.h"

static GL::Shader shader;
static GL::VertexBuffer buffer;

static float x = 0.0f;

static void initVertexBuffer() {
    buffer.init(GL::VertexBuffer::Attributes().addVector2().addFloat());
    GLfloat data[6][3] = {{-0.5f, -0.5f, 0.0f}, {-0.5f, 0.5f, 0.5f}, {0.5f, -0.5f, 0.5f},
                          {0.5f, 0.5f, 1.0f},   {-0.5f, 0.5f, 0.5f}, {0.5f, -0.5f, 0.5f}};
    buffer.setData(data, sizeof(data));
}

bool Game::init() {
    if (shader.compile({"assets/shaders/test.vs", "assets/shaders/test.fs"})) {
        return true;
    }
    initVertexBuffer();
    return false;
}

void Game::tick() {
    x += Input::getHorizontal() * 0.01f;
}

void Game::render(float lag) {
    glClearColor(0.0, 1.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    shader.use();
    (void)lag;
    shader.setFloat("xOffset", x);
    buffer.drawTriangles(6);
}
