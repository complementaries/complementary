#ifndef PLAYER_H
#define PLAYER_H

#include "Ability.h"
#include "math/Vector.h"
#include "objects/Object.h"
#include "particles/ParticleSystem.h"
#include "player/Face.h"

namespace Player {
    bool init();

    bool isColliding(Face face);
    bool isColliding(const ObjectBase& o);
    bool isCollidingWithAnyObject();
    void addForce(const Vector& force);
    void addForce(Face face, float force);
    void addBaseVelocity(const Vector& v);
    void moveForced(const Vector& v);
    void restart();
    void kill();
    bool isDead();
    bool isAllowedToMove();
    void setAllowedToMove(bool value);
    void setGravityEnabled(bool value);
    void resetVelocity();
    void resetDash();
    void setOverrideColor(Color color);
    Color getOverrideColor();
    void resetOverrideColor();
    void setHidden(bool hidden);
    bool invertColors();
    void toggleWorld();
    Vector getPosition();
    void setPosition(const Vector& pos);
    Vector getCenter(float lag = 0.0f);
    Vector getVelocity();

    void setAbilities(Ability dark, Ability light, bool animate);
    bool hasAbility(Ability a);
    Ability getAbility();
    Ability getPassiveAbility();
    bool isDashing();
    bool isGliding();

    void tick();
    void render(float lag);
    void renderImGui();

    void load();
    void save();
}

namespace PlayerParticles {
    void setParticleVelocities(std::shared_ptr<ParticleSystem> particles, int xMinSign,
                               int xMaxSign, int yMinMSign,
                               int yMaxSign); // set sign variables to adjust particle directions
    void setParticlePosition(std::shared_ptr<ParticleSystem> particles, int xCoord, int yCoord,
                             float xOffset,
                             float yOffset); // x and y coords relative to player center (top/left:
                                             // -1, bottom/right: 1, center: 0)

    void setParticleColor(std::shared_ptr<ParticleSystem> particles); // player color

    void setParticleColors();
}

#endif
