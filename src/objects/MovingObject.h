#ifndef MOVINGOBJECT_H
#define MOVINGOBJECT_H

#include "Object.h"
#include "math/Vector.h"
#include "player/Ability.h"

#include <memory>

struct MovingObjectData {
    Vector size;
    Vector goal;
    float speed;
    bool spiky[4];
};

class MovingObject : public Object<MovingObjectData> {
  public:
    MovingObject();
    MovingObject(const MovingObjectData& data);
    MovingObject(const Vector& size, const Vector& goal, float speed);

    void postInit() override;
    void reset() override;

    void tick() override;
    bool isSolid() const override;
    void onFaceCollision(Face playerFace) override;
    bool collidesWith(const Vector& position, const Vector& size) const override;
    void render(float lag) override;
    void renderEditor(float lag, bool inPalette) override;
    void render(float lag, Color color);
    std::shared_ptr<ObjectBase> clone() override;
    Vector getSize() const override;
    bool isSpiky(Face f) const;
    bool hasMoved() const override;

#ifndef NDEBUG
    void initTileEditorData(std::vector<TileEditorProp>& props) override;
    void applyTileEditorData(float* props) override;
#endif

  protected:
    void renderAt(float lag, Color color, Vector position);
    Vector lastPosition;
    Vector velocity;
    Vector initialPosition;
    bool movingBack;
};

#endif
