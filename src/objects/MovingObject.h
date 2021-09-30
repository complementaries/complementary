#ifndef MOVINGOBJECT_H
#define MOVINGOBJECT_H

#include "Object.h"
#include "math/Vector.h"
#include "player/Ability.h"

#include <memory>

struct MovingObjectData {
    Vector size;
    Vector goalA;
    Vector goalB;
    float speed;
    bool spiky[4];
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
    void render(float lag) override;
    void renderEditor(float lag) override;
    void render(float lag, Color color);
    std::shared_ptr<ObjectBase> clone() override;
    Vector getSize() const override;

#ifndef NDEBUG
    void initTileEditorData(std::vector<TileEditorProp>& props) override;
    void applyTileEditorData(float* props) override;
#endif

  protected:
    Vector lastPosition;
    Vector velocity;
};

#endif
