#ifndef COLOROBJECT_H
#define COLOROBJECT_H

#include "Object.h"
#include "math/Vector.h"
#include "particles/ParticleSystem.h"
#include "player/Ability.h"

struct ColorObjectData {
    Vector size;
    Ability abilities[2];
};

class ColorObject : public Object<ColorObjectData> {
  public:
    inline ColorObject(){};
    ColorObject(ColorObjectData data);
    ColorObject(const Vector& position, const Vector& size, Ability a, Ability b);

    void tick() override;
    void postInit() override;
    bool isSolid() const override;
    void onFaceCollision(Face playerFace) override;
    bool collidesWith(const Vector& position, const Vector& size) const override;
    void render(float lag) override;
    std::shared_ptr<ObjectBase> clone() override;
    Vector getSize() const override;
    bool hasMoved() const override;

  private:
    std::shared_ptr<ParticleSystem> particles;

#ifndef NDEBUG
    void initTileEditorData(std::vector<TileEditorProp>& props) override;
    void applyTileEditorData(float* props) override;
#endif
};

#endif
