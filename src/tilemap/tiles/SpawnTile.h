#ifndef SPAWN_TILE_H
#define SPAWN_TILE_H

#include "Tile.h"

class SpawnTile : public Tile {
  public:
    SpawnTile();
    void renderEditor(Buffer& buffer, float x, float y) const override;
    Color getColor() const override;
};

#endif
