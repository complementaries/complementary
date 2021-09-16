#ifndef FONT_H
#define FONT_H

#include "graphics/Color.h"
#include "math/Vector.h"

namespace Font {
    bool init();
    void prepare();
    void draw(const Vector& pos, float size, Color color, const char* s);
}

#endif