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
    void addForce(const Vector& force);
    void addForce(Face face, float force);
    void addBaseVelocity(const Vector& v);
    void moveForced(const Vector& v);
    void restart();
    void kill();
    bool isAllowedToMove();
    void setAllowedToMove(bool value);
    void setGravityEnabled(bool value);
    void setOverrideColor(Color color);
    void resetOverrideColor();
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

namespace PlayerParticles {
    void setParticleVelocities(std::shared_ptr<ParticleSystem> particles, int xMinSign,
                               int xMaxSign, int yMinMSign,
                               int yMaxSign); // set sign variables to adjust particle directions
    void setParticlePosition(std::shared_ptr<ParticleSystem> particles, int xCoord, int yCoord,
                             float xOffset,
                             float yOffset); // x and y coords relative to player center (top/left:
                                             // -1, bottom/right: 1, center: 2)

    void setParticleColor(std::shared_ptr<ParticleSystem> particles,
                          bool playerColor); // either player color or inverted world color
}

#endif
