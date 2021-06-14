#ifndef TILE_H
#define TILE_H

#include "graphics/Color.h"

class Tile {
  public:
    Tile(Color color);
    Color getColor() const;
    int getId() const;

  private:
    friend class Tiles;
    int id;
    Color color;
};

#endif