#ifndef MENU_H
#define MENU_H

enum class MenuType { START, PAUSE };

namespace Menu {
    void tick();
    void render(float lag);

    bool isActive();
    void clear();
    void showStartMenu();
    void showPauseMenu();
    MenuType getType();
}

#endif
