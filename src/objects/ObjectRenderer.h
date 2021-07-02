#ifndef OBJECTRENDERER_H
#define OBJECTRENDERER_H

#include "graphics/Color.h"
#include "math/Vector.h"

namespace ObjectRenderer {
    bool init();
    void prepare();
    void drawRectangle(const Vector& position, const Vector& size, Color c);
}

#endif