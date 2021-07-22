#ifndef COLOROBJECT_H
#define COLOROBJECT_H

#include "Object.h"
#include "graphics/Color.h"
#include "math/Vector.h"
#include "player/Ability.h"

struct ColorObjectData {
    Vector size;
    Ability abilities[2];
};

class ColorObject : public Object<ColorObjectData> {
  public:
    inline ColorObject(){};
    ColorObject(ColorObjectData data);
    ColorObject(const Vector& position, const Vector& size, Ability a, Ability b);

    bool isSolid() const override;
    void onFaceCollision(Face playerFace) override;
    bool collidesWith(const Vector& position, const Vector& size) const override;
    void render(float lag) const override;
    virtual std::shared_ptr<ObjectBase> clone() override;
};

#endif
