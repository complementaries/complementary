#ifndef OBJECT_H
#define OBJECT_H

#include "player/Face.h"

class Object {
  public:
    Object();
    virtual ~Object() = default;

    virtual void onFaceCollision(Face playerFace);
    virtual void onCollision();
    virtual bool isSolid() const;
    virtual bool collidesWith(const Vector& position, const Vector& size) const;
    virtual void tick();
    virtual void render(float lag) const;
};

#endif