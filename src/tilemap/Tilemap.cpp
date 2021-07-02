#include "Tilemap.h"

#include <vector>

#include "Game.h"
#include "Tiles.h"
#include "graphics/Buffer.h"
#include "graphics/gl/Shader.h"
#include "graphics/gl/VertexBuffer.h"

static GL::Shader shader;
static GL::VertexBuffer buffer;
static int vertices = 0;
static bool dirty = true;
static int width = 0;
static int height = 0;
static std::vector<int> tiles;

bool Tilemap::init(int w, int h) {
    if (shader.compile({"assets/shaders/tilemap.vs", "assets/shaders/tilemap.fs"})) {
        return true;
    }
    width = w;
    height = h;
    tiles.resize(width * height, 0);
    buffer.init(GL::VertexBuffer::Attributes().addVector2().addRGBA());
    return false;
}

int Tilemap::getWidth() {
    return width;
}

int Tilemap::getHeight() {
    return height;
}

const Tile& Tilemap::getTile(int x, int y) {
    return Tiles::get(tiles[width * y + x]);
}

void Tilemap::setTile(int x, int y, const Tile& tile) {
    tiles[width * y + x] = tile.getId();
    dirty = true;
}

static void prepareRendering() {
    if (!dirty) {
        return;
    }
    Buffer data;
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            Tilemap::getTile(x, y).render(data, x, y);
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

void Tilemap::forceReload() {
    dirty = true;
}