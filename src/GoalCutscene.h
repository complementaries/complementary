#ifndef GOAL_CUTSCENE_H
#define GOAL_CUTSCENE_H

#include "math/Vector.h"
#include "player/Face.h"

namespace GoalCutscene {
    bool init();
    void show(Vector goalPosition, Face goalFace);
    void tick();
    void render(float lag);
    bool isActive();
}

#endif
