#ifndef PLAYER_H
#define PLAYER_H

#include "Ability.h"
#include "math/Vector.h"
#include "player/Face.h"
#include "tilemap/Tilemap.h"

namespace Player {
    bool init();

    bool isColliding(Face face);
    void addForce(const Vector& force);
    void addForce(Face face, float force);
    void kill();

    void setAbilities(Ability dark, Ability light);
    bool hasAbility(Ability a);

    void tick(const Tilemap& map);
    void render(float lag);
    void renderImGui();
}

#endif
