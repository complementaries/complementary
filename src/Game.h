#ifndef GAME_H
#define GAME_H

#include "math/Matrix.h"

namespace Game {
    bool init();
    void tick();
    void render(float lag);

    extern Matrix viewMatrix;
}

#endif
