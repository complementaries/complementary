#ifndef TILES_H
#define TILES_H

#include "tiles/SpikeTile.h"
#include "tiles/Tile.h"

class Tiles {
  public:
    static Tile AIR;
    static Tile WALL;
    static SpikeTile SPIKES;

    static void init();
    static const Tile& get(int id);

  private:
    static void addTile(Tile& tile);
};

#endif