#ifndef FACE_H
#define FACE_H

#include <array>

#include "math/Vector.h"

static constexpr int FACES = 4;
enum class Face { LEFT, RIGHT, UP, DOWN, MAX };
typedef std::array<Face, FACES> Faces;

namespace FaceUtils {
    Vector getDirection(Face face);
    const Faces& getFaces();
    Face invert(Face f);
}

#endif
