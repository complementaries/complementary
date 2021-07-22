#include "Object.h"

ObjectBase::ObjectBase() : hasWall(true) {
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

void ObjectBase::render(float lag) const {
    (void)lag;
}
