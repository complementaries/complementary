#include "DoorTile.h"

#include "player/Player.h"
#include "tilemap/Tilemap.h"
#include "tilemap/Tiles.h"

DoorTile::DoorTile() : Tile(ColorUtils::GRAY, true, "key") {
}

bool DoorTile::isSolid() const {
    return Tilemap::getKeys() != 0;
}

void DoorTile::render(Buffer& buffer, float x, float y) const {
    float minX = x;
    float minY = y;
    float maxX = minX + 1;
    float maxY = minY + 1;
    Color color = getColor();
    buffer.add(minX).add(minY).add(color);
    buffer.add(maxX - 0.1f).add(minY).add(color);
    buffer.add(minX).add(maxY - 0.1f).add(color);
    buffer.add(maxX).add(maxY).add(color);
    buffer.add(maxX).add(minY + 0.1f).add(color);
    buffer.add(minX + 0.1f).add(maxY).add(color);
}