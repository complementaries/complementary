#include "Objects.h"

#include <cassert>
#include <cstring>
#include <fstream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "objects/ObjectRenderer.h"

static std::vector<std::unique_ptr<ObjectBase>> objects;

bool Objects::init() {
    return ObjectRenderer::init();
}

void Objects::clear() {
    objects.clear();
}

void Objects::add(ObjectBase* o) {
    objects.emplace_back(o);
}

bool Objects::collidesWithAny(const Vector& position, const Vector& size) {
    for (auto& o : objects) {
        if (o->isSolid() && o->collidesWith(position, size)) {
            return true;
        }
    }
    return false;
}

bool Objects::handleFaceCollision(const Vector& position, const Vector& size, Face face) {
    bool r = false;
    for (auto& o : objects) {
        if (o->isSolid() && o->collidesWith(position, size)) {
            o->onFaceCollision(face);
            r = true;
        }
    }
    return r;
}

bool Objects::hasWallCollision(const Vector& position, const Vector& size, Face face) {
    for (auto& o : objects) {
        if (o->isSolid() && o->hasWall && o->collidesWith(position, size)) {
            return true;
        }
    }
    return false;
}

void Objects::handleCollision(const Vector& position, const Vector& size) {
    for (auto& o : objects) {
        if (o->collidesWith(position, size)) {
            o->onCollision();
        }
    }
}

void Objects::tick() {
    for (auto& o : objects) {
        o->tick();
    }
}

void Objects::render(float lag) {
    ObjectRenderer::prepare();
    for (auto& o : objects) {
        o->render(lag);
    }
}

void Objects::load(const char* path) {
    std::ifstream stream;
    stream.open(path);

    char magic[5];
    stream.read(magic, 4);
    magic[4] = 0;

    // File magic must be CMOM
    assert(strcmp(magic, "CMOM") == 0);
}

void Objects::save(const char* path) {
    static char empty[8];
    size_t objectNum = objects.size();

    std::ofstream stream;
    stream.open(path);

    stream.write("CMOM", 4);
    stream.write(empty, 8);

    std::vector<int> positions;

    for (size_t i = 0; i < objects.size(); i++) {
        int filePos = stream.tellp();
        positions.push_back(filePos);
        stream.write(objects[i]->getDataPointer(), objects[i]->getDataSize());

        filePos = stream.tellp();
        // Align to 8 bytes
        int offset = filePos % 8;
        if (offset != 0) {
            stream.write(empty, 8 - offset);
        }
    }

    // Align to 8 bytes
    int offset = stream.tellp() % 8;
    if (offset != 0) {
        stream.write(empty, 8 - offset);
    }

    int dataPos = stream.tellp();

    stream.seekp(4, std::ios_base::beg);
    stream.write((char*)&dataPos, 4);

    stream.seekp(dataPos, std::ios_base::beg);
    stream.write((char*)&objectNum, 4);
    for (size_t i = 0; i < objectNum; i++) {
        size_t typeId = typeid(objects[i]).hash_code();
        stream.write((char*)&typeId, 8);
        stream.write((char*)&positions[i], 4);
        printf("Saving object %zu with typeId %zu at position %d\n", i, typeId, positions[i]);
    }
}
