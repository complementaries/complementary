#ifndef TILE_H
#define TILE_H

#include "graphics/Color.h"
#include "player/Face.h"

class Tile {
  public:
    Tile(Color color, bool solid);
    virtual ~Tile() = default;
    Color getColor() const;
    int getId() const;

    virtual void onFaceCollision(Face playerFace) const;
    virtual void onCollision() const;
    virtual bool isSolid() const;

  private:
    friend class Tiles;
    int id;
    Color color;
    bool solid;
};

#endif