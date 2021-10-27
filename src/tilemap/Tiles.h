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
    static SpikeTile SPIKES_LEFT;
    static SpikeTile SPIKES_RIGHT;
    static SpikeTile SPIKES_UP;
    static SpikeTile SPIKES_DOWN;
    static SpawnTile SPAWN_POINT;
    static GoalTile GOAL_LEFT;
    static GoalTile GOAL_RIGHT;
    static GoalTile GOAL_UP;
    static GoalTile GOAL_DOWN;

    static void init();
    static const Tile& get(char id);
    static size_t getCount();

  private:
    static void addTile(Tile& tile);
};

#endif
