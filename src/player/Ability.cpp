#include "Ability.h"

Color AbilityUtils::getColor(Ability a) {
    switch (a) {
        case Ability::DOUBLE_JUMP: return ColorUtils::rgba(192, 0, 192);
        case Ability::GLIDER: return ColorUtils::rgba(63, 255, 63);
        case Ability::DASH: return ColorUtils::rgba(255, 165, 0);
        case Ability::WALL_JUMP: return ColorUtils::rgba(0, 90, 255);
        default: return ColorUtils::GRAY;
    }
}
