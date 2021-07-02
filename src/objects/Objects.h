#ifndef OBJECTS_H
#define OBJECTS_H

#include "Object.h"

namespace Objects {
    bool init();

    void clear();
    void add(Object* o);

    bool collidesWithAny(const Vector& position, const Vector& size);
    bool handleFaceCollision(const Vector& position, const Vector& size, Face face);
    void handleCollision(const Vector& position, const Vector& size);

    void tick();
    void render(float lag);
}

#endif