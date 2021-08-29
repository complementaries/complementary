#ifndef OBJECT_H
#define OBJECT_H

#include <cstddef>
#include <memory>
#include <vector>

#include "TileEditorProp.h"
#include "player/Face.h"

class ObjectBase {
  public:
    ObjectBase();
    virtual ~ObjectBase() = default;

    virtual void onFaceCollision(Face playerFace);
    virtual void onCollision();
    virtual bool isSolid() const;
    virtual bool collidesWith(const Vector& position, const Vector& size) const;
    virtual void tick();
    virtual void render(float lag) const;
    virtual char* getDataPointer() = 0;
    virtual size_t getDataSize() const = 0;
    virtual std::shared_ptr<ObjectBase> clone() = 0;

#ifndef NDEBUG
    inline virtual void initTileEditorData(std::vector<TileEditorProp>& props) {
    }
    inline virtual void applyTileEditorData(float* props) {
    }
    inline std::vector<TileEditorProp>& getTileEditorProps() {
        return tileEditorProps;
    }

  private:
    std::vector<TileEditorProp> tileEditorProps;

  public:
#endif

    bool hasWall;
    int prototypeId = -1;
    Vector position;
};

template <typename T>
class Object : public ObjectBase {
  public:
    char* getDataPointer() {
        return reinterpret_cast<char*>(&data);
    }

    size_t getDataSize() const {
        return sizeof(T);
    }

    T data;
};

#endif
