#ifndef TILES_H
#define TILES_H

#include "tiles/BoxTile.h"
#include "tiles/GoalTile.h"
#include "tiles/SpawnTile.h"
#include "tiles/SpikeTile.h"
#include "tiles/Tile.h"

class Tiles {
  public:
    static Tile AIR;
    static BoxTile WALL;
    static SpikeTile SPIKES;
    static SpawnTile SPAWN_POINT;
    static GoalTile GOAL;

    static void init();
    static const Tile& get(char id);
    static size_t getCount();

  private:
    static void addTile(Tile& tile);
};

#endif
