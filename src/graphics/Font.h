#ifndef FONT_H
#define FONT_H

#include "graphics/Color.h"
#include "math/Matrix.h"
#include "math/Vector.h"

namespace Font {
    bool init();
    void prepare(float zLayer = 0.0f);
    void prepare(const Matrix& view, float zLayer = 0.0f);
    void draw(const Vector& pos, float size, Color color, const char* s);
    float getWidth(float size, const char* s);
}

#endif