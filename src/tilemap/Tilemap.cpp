#include "Tilemap.h"
#include "Game.h"
#include "Tiles.h"
#include "graphics/Buffer.h"

bool Tilemap::dirty = true;

void Tilemap::setDirty() {
    dirty = true;
}

Tilemap::Tilemap() : vertices(0), width(0), height(0) {
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

void Tilemap::prepareRendering() {
    if (!dirty) {
        return;
    }
    Buffer data;
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            getTile(x, y).render(data, x, y);
        }
    }
    vertices = data.getSize() / (sizeof(float) * 2 + 4);
    buffer.setData(data.getData(), data.getSize());
    dirty = false;
}

void Tilemap::render() {
    shader.use();
    shader.setMatrix("view", Game::viewMatrix);
    prepareRendering();
    buffer.drawTriangles(vertices);
}