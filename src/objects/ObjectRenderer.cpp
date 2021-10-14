#include "ObjectRenderer.h"

#include "graphics/Buffer.h"
#include "graphics/RenderState.h"
#include "graphics/gl/Shader.h"
#include "graphics/gl/VertexBuffer.h"

static GL::Shader shader;
static GL::VertexBuffer buffer;
static Buffer data;

bool ObjectRenderer::init() {
    if (shader.compile({"assets/shaders/object.vs", "assets/shaders/object.fs"})) {
        return true;
    }
    buffer.init(GL::VertexBuffer::Attributes().addVector2().addRGBA());
    return false;
}

void ObjectRenderer::prepare() {
    shader.use();
    setZ(-0.4f);
    RenderState::setViewMatrix(shader);
}

void ObjectRenderer::prepare(const Matrix& view) {
    shader.use();
    setZ(-1.0f);
    shader.setMatrix("view", view);
}

void ObjectRenderer::drawTriangle(const Vector& x, const Vector& y, const Vector& z, Color c) {
    drawTriangle(x, y, z, c, c, c);
}

void ObjectRenderer::drawTriangle(const Vector& x, const Vector& y, const Vector& z, Color xc,
                                  Color yc, Color zc) {
    data.clear();
    data.add(x).add(xc);
    data.add(y).add(yc);
    data.add(z).add(zc);
    buffer.setData(data.getData(), data.getSize());
    buffer.drawTriangles(3);
}

void ObjectRenderer::drawRectangle(const Vector& position, const Vector& size, Color c) {
    data.clear();
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

void ObjectRenderer::setZ(float z) {
    shader.setFloat("zLayer", z);
}