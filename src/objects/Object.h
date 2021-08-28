#ifndef OBJECT_H
#define OBJECT_H

#include <cstddef>
#include <memory>

#include "player/Face.h"

#ifndef NDEBUG
#include <vector>

enum class TileEditorPropType { INT, FLOAT, BOOL };

// TODO: refactor into seperate .h and .cpp files
struct TileEditorProp {
    const char* name;
    TileEditorPropType type;
    float value;
    float min;
    float max;
    bool disabled;

    inline TileEditorProp(const char* name, TileEditorPropType type, float value, float min,
                          float max, bool disabled = false)
        : name(name), type(type), value(value), min(min), max(max), disabled(disabled) {
    }

    static inline TileEditorProp Float(const char* name, float value, float min, float max,
                                       bool disabled = false) {
        return TileEditorProp(name, TileEditorPropType::FLOAT, value, min, max, disabled);
    }

    static inline TileEditorProp Int(const char* name, int value, int min, int max,
                                     bool disabled = false) {
        return TileEditorProp(name, TileEditorPropType::INT, value, min, max, disabled);
    }

    static inline TileEditorProp Bool(const char* name, bool value, bool disabled = false) {
        return TileEditorProp(name, TileEditorPropType::INT, value, 0, 1, disabled);
    }
};
#endif

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
