#include "Tilemap.h"

#include <cassert>
#include <cstring>
#include <fstream>
#include <vector>

#include "Tiles.h"
#include "graphics/Buffer.h"
#include "graphics/RenderState.h"
#include "graphics/gl/Shader.h"
#include "graphics/gl/VertexBuffer.h"
#include "player/Player.h"

static GL::Shader shader;
static GL::VertexBuffer buffer;
static GL::VertexBuffer background;
static int vertices = 0;
static int verticesTransparent = 0;
static bool dirty = true;
static int width = 0;
static int height = 0;
static std::vector<char> tiles;

bool Tilemap::init(int w, int h) {
    if (shader.compile({"assets/shaders/tilemap.vs", "assets/shaders/tilemap.fs"})) {
        return true;
    }
    width = w;
    height = h;
    tiles.resize(width * height, 0);
    buffer.init(GL::VertexBuffer::Attributes().addVector3().addRGBA());
    background.init(GL::VertexBuffer::Attributes().addVector3().addRGBA());
    return false;
}

int Tilemap::getWidth() {
    return width;
}

int Tilemap::getHeight() {
    return height;
}

Vector Tilemap::getSize() {
    return Vector(width, height);
}

void Tilemap::setSize(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
    tiles.resize(newWidth * newHeight);
}

const Tile& Tilemap::getTile(int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return Tiles::get(-1);
    }
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
    static Buffer data;
    data.clear();

    Color c = Tiles::AIR.getColor();
    data.add(0.0f).add(0.0f).add(0.0f).add(c);
    data.add(static_cast<float>(width)).add(0.0f).add(0.0f).add(c);
    data.add(0.0f).add(static_cast<float>(height)).add(0.0f).add(c);
    data.add(static_cast<float>(width)).add(static_cast<float>(height)).add(0.0f).add(c);
    data.add(static_cast<float>(width)).add(0.0f).add(0.0f).add(c);
    data.add(0.0f).add(static_cast<float>(height)).add(0.0f).add(c);
    background.setStaticData(data.getData(), data.getSize());
    data.clear();

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            Tilemap::getTile(x, y).render(data, x, y, -0.2f);
        }
    }
    vertices = data.getSize() / (sizeof(float) * 3 + 4);
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            Tilemap::getTile(x, y).renderTransparent(data, x, y, -0.5f);
        }
    }
    verticesTransparent = data.getSize() / (sizeof(float) * 3 + 4) - vertices;
    buffer.setStaticData(data.getData(), data.getSize());
    dirty = false;
}

void Tilemap::renderBackground() {
    shader.use();
    RenderState::setViewMatrix(shader);
    prepareRendering();
    background.drawTriangles(6);
}

void Tilemap::render() {
    shader.use();
    RenderState::setViewMatrix(shader);
    prepareRendering();
    buffer.drawTriangles(vertices);
}

void Tilemap::renderForeground() {
    shader.use();
    RenderState::setViewMatrix(shader);
    prepareRendering();
    buffer.drawTriangles(verticesTransparent, vertices);
}

void Tilemap::forceReload() {
    dirty = true;
}

bool Tilemap::load(const char* path) {
    std::ifstream stream;
    stream.open(path, std::ios::binary);
    if (stream.fail()) {
        return true;
    }

    char magic[5];
    stream.read(magic, 4);
    magic[4] = 0;

    // File magic must be CMTM
    assert(strcmp(magic, "CMTM") == 0);

    stream.read((char*)&width, 4);
    stream.read((char*)&height, 4);

    tiles.resize(width * height);
    stream.read(tiles.data(), width * height);
    stream.close();

    forceReload();

    Player::setPosition(getSpawnPoint());
    Player::setAbilities(Ability::NONE, Ability::NONE, false);
    return false;
}

Vector Tilemap::getSpawnPoint() {
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            if (getTile(x, y) == Tiles::SPAWN_POINT) {
                return Vector(x, y);
            }
        }
    }
    return Vector();
}

bool Tilemap::save(const char* path) {
    std::ofstream stream;
    stream.open(path, std::ios::binary);
    if (stream.fail()) {
        return true;
    }

    stream.write("CMTM", 4);
    stream.write((const char*)&width, 4);
    stream.write((const char*)&height, 4);
    stream.write(tiles.data(), width * height);

    stream.close();
    return false;
}
