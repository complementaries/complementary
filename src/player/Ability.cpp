#include "Ability.h"

Color AbilityUtils::getColor(Ability a) {
    switch (a) {
        case DOUBLE_JUMP: return ColorUtils::rgba(192, 0, 192);
        case GLIDER: return ColorUtils::rgba(63, 255, 63);
        case DASH: return ColorUtils::rgba(0, 90, 255);
        case WALL_JUMP: return ColorUtils::rgba(255, 165, 0);
        default: return ColorUtils::GRAY;
    }
}