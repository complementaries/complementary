#ifndef SAVEGAME_H
#define SAVEGAME_H

#include "player/Ability.h"
#include <cstdint>

namespace Savegame {
    struct Data {
        uint32_t completedLevels;
        uint32_t unlockedAbilities; // bit mask
    };

    bool init();
    void load();
    void save();
    void reset();
    void unlockAbilities(Ability primary, Ability secondary);
    bool abilitiesUnlocked(Ability primary, Ability secondary);
    int getCompletedLevels();
    void setCompletedLevels(int amount);
}

#endif
