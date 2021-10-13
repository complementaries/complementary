#include "Face.h"

static Faces faces{Face::LEFT, Face::RIGHT, Face::UP, Face::DOWN};

Vector FaceUtils::getDirection(Face face) {
    switch (face) {
        case Face::LEFT: return Vector(-1.0f, 0.0f);
        case Face::RIGHT: return Vector(1.0f, 0.0f);
        case Face::UP: return Vector(0.0f, -1.0f);
        case Face::DOWN: return Vector(0.0f, 1.0f);
        default: return Vector();
    }
}

const Faces& FaceUtils::getFaces() {
    return faces;
}

Face FaceUtils::invert(Face f) {
    switch (f) {
        case Face::LEFT: return Face::RIGHT;
        case Face::RIGHT: return Face::LEFT;
        case Face::UP: return Face::DOWN;
        case Face::DOWN: return Face::UP;
        default: return Face::UP;
    }
}
