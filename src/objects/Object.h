#ifndef OBJECT_H
#define OBJECT_H

#include "player/Face.h"

class ObjectBase {
  public:
    ObjectBase();
    virtual ~ObjectBase() = default;

    virtual void onFaceCollision(Face playerFace);
    virtual void onCollision();
    virtual bool isSolid() const;
    virtual bool collidesWith(const Vector& position, const Vector& size) const;
    virtual void tick();
    virtual void render(float lag) const;
    virtual char* getDataPointer() = 0;
    virtual size_t getDataSize() = 0;
};

template <typename T>
class Object : public ObjectBase {
  public:
    char* getDataPointer() {
        return (char*)&data;
    }
    size_t getDataSize() {
        return sizeof(T);
    }

  protected:
    T data;
};

#endif
