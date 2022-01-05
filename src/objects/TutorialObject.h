#ifndef TUTORIAL_OBJECT_H
#define TUTORIAL_OBJECT_H

#include "Object.h"
#include "math/Vector.h"
#include "particles/ParticleSystem.h"
#include "player/Ability.h"

enum class TutorialType {
    NONE,
    WORLD_SWITCH,
    JUMP,
    DASH_SWITCH_COMBO,
    DOUBLE_JUMP,
    GLIDER,
    DASH,
    WALL_JUMP,

    MAX
};

struct TutorialObjectData {
    TutorialType type;
    Vector size;
    bool instant;
};

class TutorialObject : public Object<TutorialObjectData> {
  public:
    TutorialObject(Vector position, Vector size);
    TutorialObject(TutorialObjectData data);

    void tick() override;
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
    bool shouldShowTutorial() const;
    bool checkTutorialCompleted() const;
    void getTutorialText(char* buffer, int length) const;
    int alpha;
    int ticksUntilDisplayed;
    bool completed;
    bool wasCollidingInLastFrame;
};

#endif
