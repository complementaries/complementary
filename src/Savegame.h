#ifndef SAVEGAME_H
#define SAVEGAME_H

#include "player/Ability.h"
#include <cstddef>
#include <cstdint>

namespace Savegame {
    constexpr int MAX_LEVEL_COUNT = 128;

    struct Data {
        uint32_t completedLevels;
        uint32_t unlockedAbilities; // bit mask
        uint32_t completionTime[MAX_LEVEL_COUNT];
        uint32_t speedrunTime;
    };

    bool init();
    void load();
    void save();
    void reset();
    void unlockAbilities(Ability primary, Ability secondary);
    bool abilitiesUnlocked(Ability primary, Ability secondary);
    int getCompletedLevels();
    void setCompletedLevels(int amount);
    uint32_t getCompletionTime(size_t levelIndex);
    void setCompletionTime(size_t levelIndex, uint32_t ticks);
    uint32_t getSpeedrunTime();
    void setSpeedrunTime(uint32_t ticks);
}

#endif
