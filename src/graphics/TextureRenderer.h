#ifndef TEXTURERENDERER_H
#define TEXTURERENDERER_H

#include "graphics/Color.h"
#include "math/Vector.h"
#include "player/Ability.h"

namespace TextureRenderer {
    bool init();
    void render(float lag);
    void renderIcon(const Vector& min, const Vector& max, Ability a, int alpha, float smooth);
}

#endif
