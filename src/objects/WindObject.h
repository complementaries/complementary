#ifndef WINDOBJECT_H
#define WINDOBJECT_H

#include "Object.h"
#include "math/Vector.h"
#include "player/Ability.h"

struct WindObjectData {
    Vector size;
    Vector force;
};

class WindObject : public Object<WindObjectData> {
  public:
    WindObject();
    WindObject(const WindObjectData& data);
    WindObject(const Vector& position, const Vector& size, const Vector& force);

    void onCollision() override;
    bool collidesWith(const Vector& position, const Vector& size) const override;
    void render(float lag) override;
    std::shared_ptr<ObjectBase> clone() override;
    Vector getSize() const override;

#ifndef NDEBUG
    void initTileEditorData(std::vector<TileEditorProp>& props) override;
    void applyTileEditorData(float* props) override;
#endif
};

#endif
