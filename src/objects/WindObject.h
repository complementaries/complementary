#ifndef WINDOBJECT_H
#define WINDOBJECT_H

#include "Object.h"
#include "math/Vector.h"
#include "particles/ParticleSystem.h"
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

    void postInit() override;
    void onCollision() override;
    bool collidesWith(const Vector& position, const Vector& size) const override;
    std::shared_ptr<ObjectBase> clone() override;
    Vector getSize() const override;
    void tick() override;
    virtual void renderEditor(float lag, bool inPalette) override;
    float calculatePlayerDistance();
    float calculatePlayerDistanceAxis(int axis);
    void handleSound(int soundId);
    static const int soundThreshold = 6.0f;

  private:
    std::shared_ptr<ParticleSystem> particles;

#ifndef NDEBUG
    void initTileEditorData(std::vector<TileEditorProp>& props) override;
    void applyTileEditorData(float* props) override;
#endif
};

#endif
