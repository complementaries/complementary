#ifndef SPIKETILE_H
#define SPIKETILE_H

#include "Tile.h"

class SpikeTile : public Tile {
  public:
    SpikeTile(Face face);

    bool isWall() const override;
    void onFaceCollision(Face playerFace) const override;
    void render(Buffer& buffer, float x, float y, float z) const override;
    void renderEditor(Buffer& buffer, float x, float y, float z) const override;

    static void addSpike(Buffer& buffer, float x, float y, float z, bool left, bool right, bool up,
                         bool down, Color c);

  private:
    Face face;
};

#endif