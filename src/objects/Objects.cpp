#include "Objects.h"

#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "objects/ColorObject.h"
#include "objects/MovingObject.h"
#include "objects/MovingSwitchObject.h"
#include "objects/ObjectRenderer.h"
#include "objects/WindObject.h"
#include "particles/ParticleSystem.h"
#include "player/Player.h"

static std::vector<std::shared_ptr<ObjectBase>> objects;
static std::vector<std::shared_ptr<ObjectBase>> prototypes;

bool Objects::init() {
    addPrototype(std::make_shared<ColorObject>(Vector(), Vector(1.0f, 1.0f), Ability::WALL_JUMP,
                                               Ability::DASH));
    addPrototype(std::make_shared<ColorObject>(Vector(), Vector(1.0f, 1.0f), Ability::DOUBLE_JUMP,
                                               Ability::GLIDER));
    addPrototype(std::make_shared<WindObject>(Vector(), Vector(1.0f, 1.0f), Vector(0.02f, 0.0f)));
    addPrototype(std::make_shared<MovingObject>(Vector(2.0f, 1.0f), Vector(5.0f, 20.0f),
                                                Vector(1.0f, 24.0f), 0.025f));
    addPrototype(std::make_shared<ParticleSystem>(Vector(24, 15)));
    addPrototype(std::make_shared<MovingSwitchObject>(Vector(2.0f, 1.0f), Vector(5.0f, 20.0f),
                                                      Vector(1.0f, 24.0f), 0.025f, true));
    addPrototype(std::make_shared<MovingSwitchObject>(Vector(2.0f, 1.0f), Vector(5.0f, 20.0f),
                                                      Vector(1.0f, 24.0f), 0.025f, false));
    return ObjectRenderer::init();
}

void Objects::addPrototype(std::shared_ptr<ObjectBase> prototype) {
    prototype->prototypeId = prototypes.size();
#ifndef NDEBUG
    prototype->initTileEditorData(prototype->getTileEditorProps());
#endif
    prototypes.push_back(std::move(prototype));
}

std::shared_ptr<ObjectBase> Objects::getPrototype(int id) {
    return prototypes[id];
}

size_t Objects::getPrototypeCount() {
    return prototypes.size();
}

void Objects::clear() {
    for (size_t i = objects.size(); i > 0;) {
        i--;
        auto object = objects[i];
        if (object->destroyOnLevelLoad) {
            objects.erase(objects.begin() + i);
        }
    }
}

void Objects::add(std::shared_ptr<ObjectBase> o) {
#ifndef NDEBUG
    o->initTileEditorData(o->getTileEditorProps());
#endif
    o->postInit();
    objects.emplace_back(o);
}

std::vector<std::shared_ptr<ObjectBase>> Objects::getObjects() {
    return objects;
}

std::shared_ptr<ObjectBase> Objects::instantiateObject(int prototypeId) {
    auto object = prototypes[prototypeId]->clone();
    object->prototypeId = prototypeId;
    add(object);
    return objects[objects.size() - 1];
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

bool Objects::hasWallCollision(const Vector& position, const Vector& size) {
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

    for (size_t i = objects.size(); i > 0;) {
        i--;
        auto object = objects[i];
        if (object->shouldDestroy) {
            objects.erase(objects.begin() + i);
        }
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
    stream.open(path, std::ios::binary);
    assert(!stream.bad());

    char magic[5];
    stream.read(magic, 4);
    magic[4] = 0;

    // File magic must be CMOM
    assert(strcmp(magic, "CMOM") == 0);

    uint64_t startPointer;
    stream.read((char*)&startPointer, 8);
    stream.seekg(startPointer, std::ios_base::beg);

    int objectNum;
    stream.read((char*)&objectNum, 4);
    printf("Reading %d objects\n", objectNum);

    for (int i = 0; i < objectNum; i++) {
        int prototypeId;
        stream.read((char*)&prototypeId, 4);
        assert(prototypeId > -1);
        auto object = instantiateObject(prototypeId);

        stream.read((char*)&object->position, sizeof(Vector));

        int dataPosition;
        stream.read((char*)&dataPosition, 4);
        int lastPos = stream.tellg();
        stream.seekg(dataPosition, std::ios_base::beg);
        object->read(stream);
        stream.seekg(lastPos, std::ios_base::beg);

#ifndef NDEBUG
        object->getTileEditorProps().clear();
        object->initTileEditorData(object->getTileEditorProps());
#endif
        object->postInit();
    }
}

void Objects::save(const char* path) {
    static char empty[8];
    size_t objectNum = objects.size();

    std::ofstream stream;
    stream.open(path, std::ios::binary);
    assert(!stream.bad());

    stream.write("CMOM", 4);
    stream.write(empty, 8);

    std::vector<int> pointers;

    for (size_t i = 0; i < objects.size(); i++) {
        int filePos = stream.tellp();
        pointers.push_back(filePos);
        objects[i]->write(stream);

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
        int prototypeId = objects[i]->prototypeId;
        assert(prototypeId > -1);
        stream.write((char*)&prototypeId, 4);
        stream.write((char*)&objects[i]->position, sizeof(Vector));
        stream.write((char*)&pointers[i], 4);
        printf("Saving object %zu with prototypeId %d at position %d\n", i, prototypeId,
               pointers[i]);
    }
}

std::shared_ptr<ObjectBase> Objects::loadObject(const char* path) {
    std::ifstream stream;
    stream.open(path, std::ios::binary);
    assert(!stream.bad());

    char magic[5];
    stream.read(magic, 4);
    magic[4] = 0;

    // File magic must be CMOB
    assert(strcmp(magic, "CMOB") == 0);

    int prototypeId;
    stream.read((char*)&prototypeId, 4);
    assert(prototypeId > -1);
    auto object = prototypes[prototypeId]->clone();
    object->prototypeId = prototypeId;

    stream.read((char*)&object->position, sizeof(Vector));
    object->read(stream);

    return object;
}

void Objects::saveObject(const char* path, ObjectBase& object) {
    std::ofstream stream;
    stream.open(path, std::ios::binary);
    assert(!stream.bad());

    stream.write("CMOB", 4);
    assert(object.prototypeId > -1);
    stream.write((char*)&object.prototypeId, sizeof(int));
    stream.write((char*)&object.position, sizeof(Vector));
    object.write(stream);
}

void Objects::print() {
    for (auto o : objects) {
        Vector v = o->getSize();
        std::cout << o->getTypeName() << " " << o->position.x << " " << o->position.y << " " << v.x
                  << " " << v.y << " " << o->shouldDestroy << "\n";
    }
}
