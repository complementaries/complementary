#include "Objects.h"

#include <memory>
#include <vector>

#include "objects/ObjectRenderer.h"

static std::vector<std::unique_ptr<Object>> objects;

bool Objects::init() {
    return ObjectRenderer::init();
}

void Objects::clear() {
    objects.clear();
}

void Objects::add(Object* o) {
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