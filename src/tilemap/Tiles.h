#ifndef TILES_H
#define TILES_H

#include "tiles/BoxTile.h"
#include "tiles/SpikeTile.h"
#include "tiles/Tile.h"

class Tiles {
  public:
    static Tile AIR;
    static BoxTile WALL;
    static SpikeTile SPIKES;

    static void init();
    static const Tile& get(char id);

  private:
    static void addTile(Tile& tile);
};

#endif
