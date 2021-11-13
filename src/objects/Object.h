#ifndef OBJECT_H
#define OBJECT_H

#include <cstddef>
#include <cstring>
#include <fstream>
#include <memory>
#include <vector>

#include "TileEditorProp.h"
#include "player/Face.h"

class ObjectBase {
  public:
    ObjectBase();
    virtual ~ObjectBase() = default;

    virtual void postInit();
    virtual void onFaceCollision(Face playerFace);
    virtual void onCollision();
    virtual bool isSolid() const;
    virtual bool isSolidInAnyWorld() const;
    virtual bool collidesWith(const Vector& position, const Vector& size) const;
    virtual void tick();
    virtual void lateTick();
    virtual void render(float lag);
    virtual void lateRender(float lag);
    virtual void renderText(float lag);
    virtual void renderEditor(float lag, bool inPalette);
    virtual void destroy();
    virtual void read(std::ifstream& in) = 0;
    virtual void write(std::ofstream& out) = 0;
    virtual std::shared_ptr<ObjectBase> clone() = 0;
    virtual Vector getSize() const;
    virtual void forceMoveParticles(const Vector& position, const Vector& size,
                                    const Vector& velocity);
    virtual void reset();
    virtual bool allowSaving() const;
    virtual bool isKeyOfType(int type) const;
    virtual bool isDoorOfType(int type) const;
    virtual void addKey();
    virtual bool hasMoved() const;

#ifndef NDEBUG
    virtual const char* getTypeName();
    virtual void renderImGui();
    inline virtual void initTileEditorData(std::vector<TileEditorProp>& props) {
    }
    inline virtual void applyTileEditorData(float* props) {
    }
    inline std::vector<TileEditorProp>& getTileEditorProps() {
        return tileEditorProps;
    }

    char filePath[128] = {};

  private:
    std::vector<TileEditorProp> tileEditorProps;

  public:
#endif

    bool hasWall;
    int prototypeId = -1;
    Vector position;
    bool isStatic;
    bool shouldDestroy;
    bool destroyOnLevelLoad;
};

template <typename T>
class Object : public ObjectBase {
  public:
    void read(std::ifstream& in) override {
        memset(reinterpret_cast<char*>(&data), 0, sizeof(T));
        in.read(reinterpret_cast<char*>(&data), sizeof(T));
    }
    void write(std::ofstream& out) override {
        out.write(reinterpret_cast<char*>(&data), sizeof(T));
    }

    T data;
};

#endif
