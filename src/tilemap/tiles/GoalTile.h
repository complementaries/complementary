#ifndef GOALTILE_H
#define GOALTILE_H

#include "Tile.h"

class GoalTile : public Tile {
  public:
    GoalTile();

    inline bool isWall() const override {
        return false;
    }
    void onCollision(int x, int y) const override;
    void render(Buffer& buffer, float x, float y) const override;
};

#endif
