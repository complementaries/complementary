#ifndef BOXTILE_H
#define BOXTILE_H

#include "Tile.h"

class BoxTile : public Tile {
  public:
    BoxTile(Color color, bool solid);

    void render(Buffer& buffer, float x, float y) const override;
    void renderEditor(Buffer& buffer, float x, float y, float z) const override;
};

#endif