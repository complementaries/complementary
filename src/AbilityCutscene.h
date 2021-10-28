#ifndef ABILITY_CUTSCENE_H
#define ABILITY_CUTSCENE_H

#include "player/Ability.h"

namespace AbilityCutscene {
    bool init();
    void show(Ability previous);
    void tick();
    void render(float lag);
    bool isActive();
}

#endif
