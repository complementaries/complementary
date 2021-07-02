#ifndef COLOROBJECT_H
#define COLOROBJECT_H

#include "Object.h"
#include "graphics/Color.h"
#include "math/Vector.h"
#include "player/Ability.h"

class ColorObject : public Object {
  public:
    ColorObject(const Vector& position, const Vector& size, Ability a, Ability b);

    bool isSolid() const override;
    void onFaceCollision(Face playerFace) override;
    bool collidesWith(const Vector& position, const Vector& size) const override;
    void render(float lag) const override;

  private:
    Vector position;
    Vector size;
    Ability abilities[2];
};

#endif