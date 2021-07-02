#include "Object.h"

Object::Object() {
}

void Object::onFaceCollision(Face playerFace) {
    (void)playerFace;
}

void Object::onCollision() {
}

bool Object::isSolid() const {
    return false;
}

bool Object::collidesWith(const Vector& position, const Vector& size) const {
    (void)position;
    (void)size;
    return false;
}

void Object::tick() {
}

void Object::render(float lag) const {
    (void)lag;
}