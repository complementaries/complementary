#ifndef MENU_H
#define MENU_H

enum class MenuType { NONE, START, PAUSE };

namespace Menu {
    void tick();
    void render(float lag);

    bool isActive();
    void clear();
    void showStartMenu();
    void showPauseMenu();
    MenuType getType();

    const char* getSwitchHelp();
    const char* getAbilityHelp();
    const char* getAbilitySwitchHelp();
    const char* getJumpHelp();
}

#endif
