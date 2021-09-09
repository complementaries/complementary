#ifndef KEYTILE_H
#define KEYTILE_H

#include "Tile.h"

class KeyTile : public Tile {
  public:
    KeyTile();

    void onCollision(int x, int y) const override;
    void render(Buffer& buffer, float x, float y) const override;
    void renderEditor(Buffer& buffer, float x, float y, float z) const override;
};

#endif