#ifndef COLOROBJECT_H
#define COLOROBJECT_H

#include "Object.h"
#include "graphics/Color.h"
#include "math/Vector.h"

class ColorObject : public Object {
  public:
    ColorObject(const Vector& position, const Vector& size, Color c);

    bool isSolid() const override;
    void onFaceCollision(Face playerFace) override;
    bool collidesWith(const Vector& position, const Vector& size) const override;
    void render(float lag) const override;

  private:
    Vector position;
    Vector size;
    Color color;
};

#endif