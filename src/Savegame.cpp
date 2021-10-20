#include "Savegame.h"

static Savegame::Data data;

bool Savegame::init() {
    data = {};
    return false;
}

void Savegame::load() {
    // TODO: implement
}

void Savegame::save() {
    // TODO: implement
}

void Savegame::unlockAbilities(Ability primary, Ability secondary) {
    data.unlockedAbilities |= 1 << static_cast<int>(primary);
    data.unlockedAbilities |= 1 << static_cast<int>(secondary);
}

bool Savegame::abilitiesUnlocked(Ability primary, Ability secondary) {
    return ((data.unlockedAbilities & (1 << static_cast<int>(primary))) != 0) &&
           ((data.unlockedAbilities & (1 << static_cast<int>(secondary))) != 0);
}
