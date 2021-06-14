#ifndef TILES_H
#define TILES_H

#include "Tile.h"

class Tiles {
  public:
    static Tile AIR;
    static Tile WALL;

    static void init();
    static const Tile& get(int id);

  private:
    static void addTile(Tile& tile);
};

#endif