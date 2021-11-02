#ifndef LEVEL_TAG_OBJECT_H
#define LEVEL_TAG_OBJECT_H

#include "Object.h"
#include "math/Vector.h"
#include "particles/ParticleSystem.h"
#include "player/Ability.h"

struct LevelTagObjectData {
    int level;
    Vector size;
};

class LevelTagObject : public Object<LevelTagObjectData> {
  public:
    LevelTagObject();
    LevelTagObject(Vector position, Vector size);
    LevelTagObject(LevelTagObjectData data);

    void onCollision() override;
    bool collidesWith(const Vector& position, const Vector& size) const override;
    Vector getSize() const override;
    std::shared_ptr<ObjectBase> clone() override;
    void renderEditor(float lag, bool inPalette) override;
    void renderText(float lag) override;

  private:
#ifndef NDEBUG
    void initTileEditorData(std::vector<TileEditorProp>& props) override;
    void applyTileEditorData(float* props) override;
#endif
};

#endif
