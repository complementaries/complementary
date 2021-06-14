#include "Tilemap.h"
#include "Tiles.h"
#include "graphics/Buffer.h"

Tilemap::Tilemap() : dirty(true), width(0), height(0) {
}

bool Tilemap::init(int width, int height) {
    if (shader.compile({"assets/shaders/tilemap.vs", "assets/shaders/tilemap.fs"})) {
        return true;
    }
    Tilemap::width = width;
    Tilemap::height = height;
    tiles.resize(width * height, 0);
    buffer.init(GL::VertexBuffer::Attributes().addVector2().addRGBA());
    return false;
}

int Tilemap::getWidth() const {
    return width;
}

int Tilemap::getHeight() const {
    return height;
}

const Tile& Tilemap::getTile(int x, int y) const {
    return Tiles::get(tiles[width * y + x]);
}

void Tilemap::setTile(int x, int y, const Tile& tile) {
    tiles[width * y + x] = tile.getId();
    dirty = true;
}

static float transform(int i, int max) {
    float normalized = i / static_cast<float>(max);
    return normalized * 2.0f - 1.0f;
}

void Tilemap::prepareRendering() {
    if (!dirty) {
        return;
    }
    Buffer data;
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            float minX = transform(x, width);
            float minY = -transform(y, height);
            float maxX = minX + 2.0f / width;
            float maxY = minY - 2.0f / height;
            Color color = getTile(x, y).getColor();

            data.add(minX).add(minY).add(color);
            data.add(maxX).add(minY).add(color);
            data.add(minX).add(maxY).add(color);
            data.add(maxX).add(maxY).add(color);
            data.add(maxX).add(minY).add(color);
            data.add(minX).add(maxY).add(color);
        }
    }
    buffer.setData(data.getData(), data.getSize());
    dirty = false;
}

void Tilemap::render() {
    shader.use();
    prepareRendering();
    buffer.drawTriangles(width * height * 6);
}