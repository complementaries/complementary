#ifndef LEVELDOOROBJECT_H
#define LEVELDOOROBJECT_H

#include "DoorObject.h"

class LevelDoorObject : public DoorObject {
  public:
    LevelDoorObject();
    LevelDoorObject(DoorObjectData data);
    LevelDoorObject(const Vector& position, const Vector& size, int type);

    void open();
    bool isSolid() const override;
    void render(float lag) override;
    void reset() override;
    std::shared_ptr<ObjectBase> clone() override;

  private:
    bool closed;
};

#endif
