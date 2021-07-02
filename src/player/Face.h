#ifndef FACE_H
#define FACE_H

#include <array>

#include "math/Vector.h"

static constexpr int FACES = 4;
enum Face { LEFT, RIGHT, UP, DOWN };
typedef std::array<Face, FACES> Faces;

namespace FaceUtils {
    Vector getDirection(Face face);
    const Faces& getFaces();
}

#endif