#ifndef KEYOBJECT_H
#define KEYOBJECT_H

#include "Object.h"
#include "math/Vector.h"
#include "particles/ParticleSystem.h"
#include "player/Ability.h"

struct KeyObjectData {
    int type;
};

class KeyObject : public Object<KeyObjectData> {
  public:
    KeyObject();
    KeyObject(KeyObjectData data);
    KeyObject(const Vector& position, int type);

    void postInit() override;
    void tick() override;
    void onCollision() override;
    bool collidesWith(const Vector& position, const Vector& size) const override;
    void lateRender(float lag) override;
    void renderEditor(float lag, bool inPalette) override;
    std::shared_ptr<ObjectBase> clone() override;
    Vector getSize() const override;
    void reset() override;
    bool isKeyOfType(int type) const override;

  private:
    std::shared_ptr<ParticleSystem> particles;
#ifndef NDEBUG
    void initTileEditorData(std::vector<TileEditorProp>& props) override;
    void applyTileEditorData(float* props) override;
#endif

  private:
    void renderColor(float lag, Color color);

    Vector goal;
    Vector lastRenderPosition;
    Vector renderPosition;
    int counter;
    int alpha;
    bool collected;
    bool added;
};

#endif
