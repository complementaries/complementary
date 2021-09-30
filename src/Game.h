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

    void fadeIn();
    void fadeOut();
    bool isFading();
}

#endif
