#ifndef MENU_H
#define MENU_H

#include <cstdint>

enum class MenuType { NONE, START, SPEEDRUN, PAUSE };

namespace Menu {
    void tick();
    void render(float lag);

    bool isActive();
    void clear();
    void showStartMenu();
    void showPauseMenu();
    void showSpeedrunMenu(bool isNewRecord, uint64_t oldRecord);
    void showCredits();
    MenuType getType();

    const char* getSwitchHelp();
    const char* getAbilityHelp();
    const char* getAbilitySwitchHelp();
    const char* getJumpHelp();
}

#endif
