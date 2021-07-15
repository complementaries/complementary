#include <memory>
#include <vector>

#include "Tiles.h"

static Tile defaultTile{ColorUtils::rgba(0xFF, 0x66, 0x00), false};
static std::vector<Tile*> tiles;

Tile Tiles::AIR{ColorUtils::rgba(0xFF, 0xFF, 0xFF), false};
BoxTile Tiles::WALL{ColorUtils::rgba(0x00, 0x00, 0x00), true};
SpikeTile Tiles::SPIKES;
KeyTile Tiles::KEY;
Tile Tiles::COLLECTED_KEY = AIR;
DoorTile Tiles::DOOR;

void Tiles::addTile(Tile& tile) {
    tile.id = tiles.size();
    tiles.push_back(&tile);
}

void Tiles::init() {
    addTile(AIR);
    addTile(WALL);
    addTile(SPIKES);
    addTile(KEY);
    addTile(COLLECTED_KEY);
    addTile(DOOR);
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
