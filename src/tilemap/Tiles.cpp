#include <memory>
#include <vector>

#include "Tiles.h"

static Tile defaultTile{ColorUtils::rgba(0xFF, 0x66, 0x00), false};
static std::vector<Tile*> tiles;

Tile Tiles::AIR{ColorUtils::rgba(0xFF, 0xFF, 0xFF), false};
BoxTile Tiles::WALL{ColorUtils::rgba(0x00, 0x00, 0x00), true};
SpikeTile Tiles::SPIKES;

void Tiles::addTile(Tile& tile) {
    tile.id = tiles.size();
    tiles.push_back(&tile);
}

void Tiles::init() {
    addTile(AIR);
    addTile(WALL);
    addTile(SPIKES);
}

const Tile& Tiles::get(int id) {
    if (id < 0 || static_cast<unsigned int>(id) >= tiles.size()) {
        return defaultTile;
    }
    return *(tiles[id]);
}