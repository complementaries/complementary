#ifndef MOVINGSWITCHOBJECT_H
#define MOVINGSWITCHOBJECT_H

#include "MovingObject.h"
#include "particles/ParticleSystem.h"

class MovingSwitchObject : public MovingObject {
  public:
    MovingSwitchObject();
    MovingSwitchObject(const Vector& size, const Vector& goal, float speed, bool seen);

    void tick() override;
    void postInit() override;
    bool isSolid() const override;
    bool isSolidInAnyWorld() const override;
    void render(float lag) override;
    void renderEditor(float lag, bool inPalette) override;
    std::shared_ptr<ObjectBase> clone() override;
    void read(std::ifstream& in) override;
    void write(std::ofstream& out) override;

  private:
    bool seen;
    std::shared_ptr<ParticleSystem> hiddenParticles;
    std::shared_ptr<ParticleSystem> seenParticles;
};

#endif
