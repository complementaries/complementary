#ifndef GAME_H
#define GAME_H

#include "math/Matrix.h"

namespace Game {
    bool init();
    void tick();
    void render(float lag);
    void renderImGui();

    void onWindowResize(int width, int height);

    extern Matrix viewMatrix;
}

#endif
