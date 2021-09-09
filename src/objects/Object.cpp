#include "Object.h"

ObjectBase::ObjectBase() : hasWall(true) {
}

void ObjectBase::postInit() {
}

void ObjectBase::onFaceCollision(Face playerFace) {
    (void)playerFace;
}

void ObjectBase::onCollision() {
}

bool ObjectBase::isSolid() const {
    return false;
}

bool ObjectBase::collidesWith(const Vector& position, const Vector& size) const {
    (void)position;
    (void)size;
    return false;
}

void ObjectBase::tick() {
}

void ObjectBase::render(float lag) {
    (void)lag;
}

void ObjectBase::renderEditor(float lag) {
    render(lag);
}

#ifndef NDEBUG
const char* ObjectBase::getTypeName() {
    auto mangledName = typeid(*this).name();
    // The mangled name encodes the name length before the actual name, get rid of numbers at the
    // beginning
    while (*mangledName < 'A') {
        mangledName++;
    }
    return mangledName;
}

void ObjectBase::renderImGui() {
}
#endif

void ObjectBase::destroy() {
    shouldDestroy = true;
}
