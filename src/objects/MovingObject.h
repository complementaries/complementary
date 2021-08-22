#ifndef MOVINGOBJECT_H
#define MOVINGOBJECT_H

#include "Object.h"
#include "math/Vector.h"
#include "player/Ability.h"

struct MovingObjectData {
    Vector size;
    Vector goalA;
    Vector goalB;
    float speed;
};

class MovingObject : public Object<MovingObjectData> {
  public:
    MovingObject();
    MovingObject(const MovingObjectData& data);
    MovingObject(const Vector& size, const Vector& a, const Vector& b, float speed);

    void tick() override;
    bool isSolid() const override;
    void onFaceCollision(Face playerFace) override;
    bool collidesWith(const Vector& position, const Vector& size) const override;
    void render(float lag) const override;
    std::shared_ptr<ObjectBase> clone() override;

  private:
    Vector lastPosition;
    Vector velocity;
};

#endif
