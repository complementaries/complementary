#ifndef OBJECTS_H
#define OBJECTS_H

#include "Object.h"

namespace Objects {
    bool init();

    void clear();
    void add(ObjectBase* o);

    bool collidesWithAny(const Vector& position, const Vector& size);
    bool handleFaceCollision(const Vector& position, const Vector& size, Face face);
    bool hasWallCollision(const Vector& position, const Vector& size, Face face);
    void handleCollision(const Vector& position, const Vector& size);

    void tick();
    void render(float lag);

    void load(const char* path);
    void save(const char* path);
}

#endif
