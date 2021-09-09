#ifndef TILE_H
#define TILE_H

#include "graphics/Buffer.h"
#include "graphics/Color.h"
#include "player/Face.h"

class Tile {
  public:
    Tile(Color color, bool solid, const char* editorGroup);
    virtual ~Tile() = default;
    virtual Color getColor() const;
    char getId() const;

    virtual void onFaceCollision(Face playerFace) const;
    virtual void onCollision(int x, int y) const;
    virtual bool isSolid() const;
    virtual bool isWall() const;
    virtual const char* getEditorGroup() const;
    virtual void render(Buffer& buffer, float x, float y) const;
    virtual void renderEditor(Buffer& buffer, float x, float y, float z) const;

    bool operator==(const Tile& other) const;

  private:
    friend class Tiles;
    char id;
    Color color;
    bool solid;
    const char* editorGroup;
};

#endif
