#ifndef WINDOW_H
#define WINDOW_H

namespace Window {
    constexpr float SECONDS_PER_TICK = 0.01f;

    bool init();
    void run();
    void exit();

    int getWidth();
    int getHeight();
    int getDrawableWidth();
    int getDrawableHeight();

    void toggleFullscreen();
    bool isFullscreen();
}

#endif
