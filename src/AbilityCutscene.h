#ifndef ABILITY_OVERLAY_H
#define ABILITY_OVERLAY_H

#include "player/Ability.h"

namespace AbilityCutscene {
    bool init();
    void show(Ability previous);
    void tick();
    void render(float lag);
    bool isActive();
}

#endif
