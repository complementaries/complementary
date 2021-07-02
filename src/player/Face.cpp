#include "Face.h"

static Faces faces{LEFT, RIGHT, UP, DOWN};

Vector FaceUtils::getDirection(Face face) {
    switch (face) {
        case LEFT: return Vector(-1.0f, 0.0f);
        case RIGHT: return Vector(1.0f, 0.0f);
        case UP: return Vector(0.0f, -1.0f);
        case DOWN: return Vector(0.0f, 1.0f);
        default: return Vector();
    }
}

const Faces& FaceUtils::getFaces() {
    return faces;
}