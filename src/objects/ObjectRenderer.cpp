#include "ObjectRenderer.h"

#include "Game.h"
#include "graphics/Buffer.h"
#include "graphics/gl/Shader.h"
#include "graphics/gl/VertexBuffer.h"

static GL::Shader shader;
static GL::VertexBuffer buffer;

bool ObjectRenderer::init() {
    if (shader.compile({"assets/shaders/object.vs", "assets/shaders/object.fs"})) {
        return true;
    }
    buffer.init(GL::VertexBuffer::Attributes().addVector2().addRGBA());
    return false;
}

void ObjectRenderer::prepare() {
    shader.use();
    shader.setMatrix("view", Game::viewMatrix);
}

void ObjectRenderer::drawRectangle(const Vector& position, const Vector& size, Color c) {
    Buffer data;
    float minX = position[0];
    float minY = position[1];
    float maxX = minX + size[0];
    float maxY = minY + size[1];

    data.add(minX).add(minY).add(c);
    data.add(maxX).add(minY).add(c);
    data.add(minX).add(maxY).add(c);
    data.add(maxX).add(maxY).add(c);
    data.add(maxX).add(minY).add(c);
    data.add(minX).add(maxY).add(c);

    buffer.setData(data.getData(), data.getSize());
    buffer.drawTriangles(6);
}