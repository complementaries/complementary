#ifndef SPIKETILE_H
#define SPIKETILE_H

#include "Tile.h"

class SpikeTile : public Tile {
  public:
    SpikeTile();

    void onFaceCollision(Face playerFace) const override;
    void render(Buffer& buffer, float x, float y) const override;
};

#endif