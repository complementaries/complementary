#ifndef ABILITY_H
#define ABILITY_H

#include "graphics/Color.h"

enum Ability { NONE, DOUBLE_JUMP, GLIDER, DASH, WALL_JUMP };

namespace AbilityUtils {
    Color getColor(Ability a);
}

#endif
