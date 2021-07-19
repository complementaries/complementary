#ifndef PARTICLES_H
#define PARTICLES_H

#include "graphics/Color.h"
#include "math/Vector.h"

namespace Particles {
    bool init();
    void tick();
    void render(float lag);

    void spawnTriangle(const Vector& position, const Vector& velocity, float gravity, int lifetime,
                       Color startColor, Color endColor, float startSize, float endSize);
    void spawnSquare(const Vector& position, const Vector& velocity, float gravity, int lifetime,
                     Color startColor, Color endColor, float startSize, float endSize);
    void spawnCircle(const Vector& position, const Vector& velocity, float gravity, int lifetime,
                     Color startColor, Color endColor, float startSize, float endSize);
}

#endif