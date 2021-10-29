#ifndef GAME_H
#define GAME_H

namespace Game {
    bool init();
    void tick();
    void render(float lag);
    void renderImGui();
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
}

#endif
