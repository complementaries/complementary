#include "Tilemap.h"

#include <cassert>
#include <cstring>
#include <fstream>
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
static std::vector<char> tiles;
static int keys = 0;

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
    int index = width * y + x;
    keys += (tile == Tiles::KEY) - (tiles[index] == Tiles::KEY.getId());
    tiles[index] = tile.getId();
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

void Tilemap::load(const char* path) {
    std::ifstream stream;
    stream.open(path);

    char magic[5];
    stream.read(magic, 4);
    magic[4] = 0;

    // File magic must be CMTM
    assert(strcmp(magic, "CMTM") == 0);

    stream.read((char*)&width, 4);
    stream.read((char*)&height, 4);

    stream.read(tiles.data(), width * height);
    forceReload();

    keys = 0;
    for (char c : tiles) {
        keys += c == Tiles::KEY.getId();
    }
}

void Tilemap::save(const char* path) {
    std::ofstream stream;
    stream.open(path);

    stream.write("CMTM", 4);
    stream.write((const char*)&width, 4);
    stream.write((const char*)&height, 4);
    stream.write(tiles.data(), width * height);
}

void Tilemap::reset() {
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            if (getTile(x, y) == Tiles::COLLECTED_KEY) {
                setTile(x, y, Tiles::KEY);
            }
        }
    }
}

int Tilemap::getKeys() {
    return keys;
}