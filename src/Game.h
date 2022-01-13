#ifndef GAME_H
#define GAME_H

#include "math/Vector.h"

enum class GameMode {
    DEFAULT,
    SPEEDRUN,
};

namespace Game {
    bool init();
    void tick();
    void render(float lag);
#ifndef NDEBUG
    void renderImGui();
#endif
    GameMode getMode();
    void setNextLevelIndex(int index);
    void nextLevel();
    void loadTitleScreen();
    void exitTitleScreen(GameMode mode);
    bool inTitleScreen();
    bool loadLevelSelect();
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
    void setBackgroundParticleColor();

    bool canEnterLevel();
    int levelStartAlpha();

    int64_t getTimerTicks();
}

#endif
