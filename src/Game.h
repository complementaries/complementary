#ifndef GAME_H
#define GAME_H

namespace Game {
    bool init();
    void tick();
    void render(float lag);
    void renderImGui();
    void nextLevel();

    void onWindowResize(int width, int height);
    void onMouseEvent(void* eventPointer);

    void fadeIn(int speed = 1);
    void fadeOut(int speed = 1);
    bool isFading();
}

#endif
