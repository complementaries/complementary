#ifndef MENU_H
#define MENU_H

namespace Menu {
    void tick();
    void render(float lag);

    bool isActive();
    void clear();
    void showStartMenu();
    void showPauseMenu();
}

#endif