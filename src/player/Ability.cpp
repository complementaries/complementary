#include "Ability.h"
#include "Menu.h"

#include <cstdio>

Color AbilityUtils::getColor(Ability a) {
    switch (a) {
        case Ability::DOUBLE_JUMP: return ColorUtils::rgba(192, 0, 192);
        case Ability::GLIDER: return ColorUtils::rgba(63, 255, 63);
        case Ability::DASH: return ColorUtils::rgba(255, 165, 0);
        case Ability::WALL_JUMP: return ColorUtils::rgba(0, 90, 255);
        default: return ColorUtils::GRAY;
    }
}

const char* AbilityUtils::getName(Ability a) {
    switch (a) {
        case Ability::DOUBLE_JUMP: return "Double Jump";
        case Ability::GLIDER: return "Glider";
        case Ability::DASH: return "Dash";
        case Ability::WALL_JUMP: return "Wall Jump";
        case Ability::NONE: return "None";
        default: return "???";
    }
}

bool AbilityUtils::getDescription(Ability a, char* buffer, int length) {
    switch (a) {
        case Ability::DOUBLE_JUMP: return false;
        case Ability::GLIDER:
            snprintf(buffer, length, "Hold [%s] to glide", Menu::getAbilityHelp());
            return true;
        case Ability::DASH:
            snprintf(buffer, length, "Press [%s] to dash", Menu::getAbilityHelp());
            return true;
        case Ability::WALL_JUMP: return false;
        case Ability::NONE: return false;
        default: return false;
    }
}
