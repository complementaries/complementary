#ifndef GOALTILE_H
#define GOALTILE_H

#include "Tile.h"
#include "objects/Objects.h"
#include "particles/ParticleSystem.h"

class GoalTile : public Tile {
  public:
    static void init();

    GoalTile(Face face);

    bool isWall() const override;
    void onCollision(int x, int y) const override;
    void renderTransparent(Buffer& buffer, float x, float y, float z) const override;
    void onLoad(int x, int y) const override;

  private:
    static std::shared_ptr<ParticleSystem> prototype;
    Face face;
};

#endif
