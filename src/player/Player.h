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
    Vector getPosition();
    void setPosition(const Vector& pos);
    Vector getCenter(float lag = 0.0f);

    void setAbilities(Ability dark, Ability light);
    bool hasAbility(Ability a);
    Ability getAbility();
    Ability getPassiveAbility();
    bool isGliding();

    void tick();
    void render(float lag);
    void renderImGui();

    void load();
    void save();
}

#endif
