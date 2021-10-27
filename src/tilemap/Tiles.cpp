#include <memory>
#include <vector>

#include "Tiles.h"

static Tile defaultTile{ColorUtils::rgba(0xFF, 0x66, 0x00), false, nullptr};
static std::vector<Tile*> tiles;

Tile Tiles::AIR{ColorUtils::rgba(0xFF, 0xFF, 0xFF), false, nullptr};
BoxTile Tiles::WALL{ColorUtils::rgba(0x00, 0x00, 0x00), true};
SpikeTile Tiles::SPIKES;
SpawnTile Tiles::SPAWN_POINT;
GoalTile Tiles::GOAL_LEFT{Face::LEFT};
GoalTile Tiles::GOAL_RIGHT{Face::RIGHT};
GoalTile Tiles::GOAL_UP{Face::UP};
GoalTile Tiles::GOAL_DOWN{Face::DOWN};

void Tiles::addTile(Tile& tile) {
    tile.id = tiles.size();
    tiles.push_back(&tile);
}

void Tiles::init() {
    addTile(AIR);
    addTile(WALL);
    addTile(SPIKES);
    addTile(SPAWN_POINT);
    addTile(GOAL_LEFT);
    addTile(GOAL_RIGHT);
    addTile(GOAL_UP);
    addTile(GOAL_DOWN);
}

const Tile& Tiles::get(char id) {
    if (id < 0 || static_cast<unsigned char>(id) >= tiles.size()) {
        return defaultTile;
    }
    return *(tiles[id]);
}

size_t Tiles::getCount() {
    return tiles.size();
}
