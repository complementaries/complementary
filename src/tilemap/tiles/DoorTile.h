#ifndef DOORTILE_H
#define DOORTILE_H

#include "Tile.h"

class DoorTile : public Tile {
  public:
    DoorTile();

    bool isSolid() const override;
    void render(Buffer& buffer, float x, float y) const override;
    void renderEditor(Buffer& buffer, float x, float y, float z) const override;
};

#endif