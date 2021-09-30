#ifndef OBJECTRENDERER_H
#define OBJECTRENDERER_H

#include "graphics/Color.h"
#include "math/Matrix.h"
#include "math/Vector.h"

namespace ObjectRenderer {
    bool init();
    void prepare();
    void prepare(const Matrix& view);
    void drawTriangle(const Vector& x, const Vector& y, const Vector& z, Color c);
    void drawRectangle(const Vector& position, const Vector& size, Color c);
    void setZ(float z);
}

#endif