#include <memory>
#include <vector>

#include "Tiles.h"

static BoxTile defaultTile{ColorUtils::rgba(0xFF, 0x66, 0x00), false};
static std::vector<Tile*> tiles;

Tile Tiles::AIR{ColorUtils::rgba(0xFF, 0xFF, 0xFF), false, nullptr};
BoxTile Tiles::WALL{ColorUtils::rgba(0x00, 0x00, 0x00), true};
SpikeTile Tiles::SPIKES_LEFT{Face::LEFT};
SpikeTile Tiles::SPIKES_RIGHT{Face::RIGHT};
SpikeTile Tiles::SPIKES_UP{Face::UP};
SpikeTile Tiles::SPIKES_DOWN{Face::DOWN};
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
    addTile(SPIKES_LEFT);
    addTile(SPIKES_RIGHT);
    addTile(SPIKES_UP);
    addTile(SPIKES_DOWN);
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
