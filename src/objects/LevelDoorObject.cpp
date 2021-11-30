#include "LevelDoorObject.h"
#include <iostream>
LevelDoorObject::LevelDoorObject() : closed(true) {
}

LevelDoorObject::LevelDoorObject(DoorObjectData data) : DoorObject(data), closed(true) {
}

LevelDoorObject::LevelDoorObject(const Vector& position, const Vector& size, int type)
    : DoorObject(position, size, type), closed(true) {
}

void LevelDoorObject::open() {
    closed = false;
}

bool LevelDoorObject::isSolid() const {
    return closed;
}

void LevelDoorObject::render(float lag) {
    if (closed) {
        DoorObject::render(lag);
    }
}

void LevelDoorObject::reset() {
    DoorObject::reset();
    closed = true;
}

std::shared_ptr<ObjectBase> LevelDoorObject::clone() {
    return std::make_shared<LevelDoorObject>(data);
}