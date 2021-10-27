#ifndef DOOROBJECT_H
#define DOOROBJECT_H

#include "Object.h"
#include "math/Vector.h"
#include "particles/ParticleSystem.h"
#include "player/Ability.h"

struct DoorObjectData {
    Vector size;
    int type;
};

class DoorObject : public Object<DoorObjectData> {
  public:
    DoorObject();
    DoorObject(DoorObjectData data);
    DoorObject(const Vector& position, const Vector& size, int type);

    bool isSolid() const override;
    bool collidesWith(const Vector& position, const Vector& size) const override;
    void tick() override;
    void postInit() override;
    void render(float lag) override;
    void renderEditor(float lag, bool inPalette) override;
    std::shared_ptr<ObjectBase> clone() override;
    Vector getSize() const override;
    bool isDoorOfType(int type) const override;
    void addKey() override;
    void reset() override;

#ifndef NDEBUG
    void initTileEditorData(std::vector<TileEditorProp>& props) override;
    void applyTileEditorData(float* props) override;
#endif

  private:
    void render(float lag, Color color);

    int maxKeys;
    int keys;
    int alpha;
    std::shared_ptr<ParticleSystem> particles;
    bool firstTick;
};

#endif
