#ifndef ABILITY_H
#define ABILITY_H

#include "graphics/Color.h"

enum class Ability { NONE, DOUBLE_JUMP, GLIDER, DASH, WALL_JUMP, MAX };

namespace AbilityUtils {
    Color getColor(Ability a);
    const char* getName(Ability a);
    bool getDescription(Ability a, char* buffer, int length);
}

#endif
