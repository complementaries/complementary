#ifndef PLAYER_H
#define PLAYER_H

#include "Ability.h"
#include "math/Vector.h"
#include "objects/Object.h"
#include "player/Face.h"

namespace Player {
    bool init();

    bool isColliding(Face face);
    bool isColliding(const ObjectBase& o);
    void addForce(const Vector& force);
    void addForce(Face face, float force);
    void addBaseVelocity(const Vector& v);
    void moveForced(const Vector& v);
    void kill();
    bool invertColors();
    void toggleWorld();
    void setPosition(const Vector& pos);

    void setAbilities(Ability dark, Ability light);
    bool hasAbility(Ability a);
    bool isGliding();

    void tick();
    void render(float lag);
    void renderImGui();

    void load();
    void save();
}

#endif
