#ifndef OBJECTRENDERER_H
#define OBJECTRENDERER_H

#include "graphics/Color.h"
#include "math/Matrix.h"
#include "math/Vector.h"

namespace ObjectRenderer {
    bool init();
    void render();
    void clearStaticBuffer();
    bool dirtyStaticBuffer();
    void renderStatic();
    void render(const Matrix& view);
    void addTriangle(const Vector& x, const Vector& y, const Vector& z, Color c);
    void addTriangle(const Vector& x, const Vector& y, const Vector& z, Color c, float zLayer);
    void addTriangle(const Vector& x, const Vector& y, const Vector& z, float zLayer, Color xc,
                     Color yc, Color zc);
    void addRectangle(const Vector& position, const Vector& size, Color c);
    void addRectangle(const Vector& position, const Vector& size, Color c, float zLayer);
    void addSpike(const Vector& position, bool left, bool right, bool up, bool down, Color c);

    void bindBuffer(bool isStatic);
    void setDefaultZ(float z);
    void resetDefaultZ();

    void setScale(const Vector& scale);
}

#endif