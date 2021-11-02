#ifndef OBJECTS_H
#define OBJECTS_H

#include "Object.h"
#include <memory>
#include <vector>

namespace Objects {
    bool init();

    void clear();
    void addPrototype(std::shared_ptr<ObjectBase> o);
    size_t getPrototypeCount();
    std::shared_ptr<ObjectBase> getPrototype(int id);

    void add(std::shared_ptr<ObjectBase> o);
    std::vector<std::shared_ptr<ObjectBase>> getObjects();
    std::shared_ptr<ObjectBase> instantiateObject(int prototypeIndex, Vector position = Vector());
    template <typename T>
    std::shared_ptr<T> instantiate(int prototypeId) {
        auto obj = instantiateObject(prototypeId);
        return std::dynamic_pointer_cast<T>(obj);
    }
    template <typename T>
    std::shared_ptr<T> instantiateClone(std::shared_ptr<T> prototype, Vector position = Vector()) {
        auto obj = prototype->clone();
        obj->prototypeId = prototype->prototypeId;
        obj->position = position;
        add(obj);
        obj->postInit();
        return std::dynamic_pointer_cast<T>(obj);
    }
    std::shared_ptr<ObjectBase> instantiateObjectNoInit(int prototypeIndex,
                                                        Vector position = Vector());

    bool collidesWithAnySolid(const Vector& position, const Vector& size);
    bool collidesWithAny(const Vector& position, const Vector& size);
    bool handleFaceCollision(const Vector& position, const Vector& size, Face face);
    bool hasWallCollision(const Vector& position, const Vector& size);
    void handleCollision(const Vector& position, const Vector& size);

    void tick();
    void lateTick();
    void render(float lag);
    void renderText(float lag);

    void load(const char* path);
    void save(const char* path);

    void forceMoveParticles(const Vector& position, const Vector& size, const Vector& velocity);

    std::shared_ptr<ObjectBase> loadObject(const char* path, Vector position = Vector());

    template <typename T>
    std::shared_ptr<T> loadObject(const char* path, Vector position = Vector()) {
        auto obj = loadObject(path, position);
        return std::dynamic_pointer_cast<T>(obj);
    }
    template <typename T>
    std::shared_ptr<T> instantiateObject(const char* path, Vector position = Vector()) {
        auto obj = loadObject<T>(path, position);
        obj->position = position;
        add(obj);
        obj->postInit();
        return obj;
    }

    void saveObject(const char* path, ObjectBase& object);
    void reset();
    std::shared_ptr<ObjectBase> findDoor(int type);
    int countKeys(int type);
}

#endif
