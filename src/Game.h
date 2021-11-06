#ifndef GAME_H
#define GAME_H

#include "math/Vector.h"

namespace Game {
    bool init();
    void tick();
    void render(float lag);
#ifndef NDEBUG
    void renderImGui();
#endif
    void setNextLevelIndex(int index);
    void nextLevel();
    void exitTitleScreen();
    bool inTitleScreen();
    void loadLevelSelect();
    void switchWorld();

    void onWindowResize(int width, int height);
    void onMouseEvent(void* eventPointer);

    void fadeIn(int speed = 1);
    void fadeOut(int speed = 1);
    void setFade(int amount);
    bool isFading();

    int getCurrentLevel();
    void setLevelScreenPosition(const Vector& v);

    void resetTickCounter();
    void setTickCounterPaused(bool paused);

    void pause();
    bool isPaused();
}

#endif
