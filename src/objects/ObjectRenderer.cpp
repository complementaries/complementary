#include "ObjectRenderer.h"

#include "graphics/Buffer.h"
#include "graphics/RenderState.h"
#include "graphics/gl/Shader.h"
#include "graphics/gl/VertexBuffer.h"

static GL::Shader shader;
static GL::VertexBuffer buffer;
static GL::VertexBuffer staticBuffer;
static Buffer data[2];
static int dataIndex = 0;
static int staticVertices = 0;
static bool dirty = true;
static float zLayer = -0.4f;

bool ObjectRenderer::init() {
    if (shader.compile({"assets/shaders/object.vs", "assets/shaders/object.fs"})) {
        return true;
    }
    buffer.init(GL::VertexBuffer::Attributes().addVector3().addRGBA());
    staticBuffer.init(GL::VertexBuffer::Attributes().addVector3().addRGBA());
    return false;
}

void ObjectRenderer::render() {
    shader.use();
    RenderState::setViewMatrix(shader);
    buffer.setData(data[0].getData(), data[0].getSize());
    buffer.drawTriangles(data[0].getSize() / (sizeof(float) * 3 + 4));
    data[0].clear();
}

void ObjectRenderer::render(const Matrix& view) {
    shader.use();
    shader.setMatrix("view", view);
    buffer.setData(data[0].getData(), data[0].getSize());
    buffer.drawTriangles(data[0].getSize() / (sizeof(float) * 3 + 4));
    data[0].clear();
}

void ObjectRenderer::clearStaticBuffer() {
    dirty = true;
}

bool ObjectRenderer::dirtyStaticBuffer() {
    return dirty;
}

void ObjectRenderer::renderStatic() {
    shader.use();
    RenderState::setViewMatrix(shader);
    if (dirty) {
        staticVertices = data[1].getSize() / (sizeof(float) * 3 + 4);
        staticBuffer.setData(data[1].getData(), data[1].getSize());
        dirty = false;
    }
    staticBuffer.drawTriangles(staticVertices);
    data[1].clear();
}

void ObjectRenderer::addTriangle(const Vector& x, const Vector& y, const Vector& z, Color c,
                                 float zLayer) {
    addTriangle(x, y, z, zLayer, c, c, c);
}

void ObjectRenderer::addTriangle(const Vector& x, const Vector& y, const Vector& z, Color c) {
    addTriangle(x, y, z, zLayer, c, c, c);
}

void ObjectRenderer::addTriangle(const Vector& x, const Vector& y, const Vector& z, float zLayer,
                                 Color xc, Color yc, Color zc) {
    data[dataIndex].add(x).add(zLayer).add(xc);
    data[dataIndex].add(y).add(zLayer).add(yc);
    data[dataIndex].add(z).add(zLayer).add(zc);
}

void ObjectRenderer::addRectangle(const Vector& position, const Vector& size, Color c,
                                  float zLayer) {
    float minX = position[0];
    float minY = position[1];
    float maxX = minX + size[0];
    float maxY = minY + size[1];

    data[dataIndex].add(minX).add(minY).add(zLayer).add(c);
    data[dataIndex].add(maxX).add(minY).add(zLayer).add(c);
    data[dataIndex].add(minX).add(maxY).add(zLayer).add(c);
    data[dataIndex].add(maxX).add(maxY).add(zLayer).add(c);
    data[dataIndex].add(maxX).add(minY).add(zLayer).add(c);
    data[dataIndex].add(minX).add(maxY).add(zLayer).add(c);
}

void ObjectRenderer::addRectangle(const Vector& position, const Vector& size, Color c) {
    addRectangle(position, size, c, zLayer);
}

void ObjectRenderer::bindBuffer(bool isStatic) {
    dataIndex = isStatic;
}

void ObjectRenderer::setDefaultZ(float z) {
    zLayer = z;
}

void ObjectRenderer::resetDefaultZ() {
    zLayer = -0.4f;
}