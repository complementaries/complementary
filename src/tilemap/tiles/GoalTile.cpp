#include "GoalTile.h"

#include "Game.h"
#include "player/Player.h"
#include "tilemap/Tilemap.h"
#include "tilemap/Tiles.h"

GoalTile::GoalTile() : Tile(ColorUtils::rgba(255, 0, 0), false, "default") {
}

void GoalTile::onCollision(int x, int y) const {
    Game::nextLevel();
}

void GoalTile::render(Buffer& buffer, float x, float y) const {
    Color color = getColor();
    buffer.add(x + 0.5f).add(y).add(color);
    buffer.add(x).add(y + 0.5f).add(color);
    buffer.add(x + 1.0f).add(y + 0.5f).add(color);
    buffer.add(x + 0.5f).add(y + 1.0f).add(color);
    buffer.add(x).add(y + 0.5f).add(color);
    buffer.add(x + 1.0f).add(y + 0.5f).add(color);
}

void GoalTile::renderEditor(Buffer& buffer, float x, float y, float z) const {
    Color color = getColor();
    buffer.add(x + 0.5f).add(y).add(z).add(color);
    buffer.add(x).add(y + 0.5f).add(z).add(color);
    buffer.add(x + 1.0f).add(y + 0.5f).add(z).add(color);
    buffer.add(x + 0.5f).add(y + 1.0f).add(z).add(color);
    buffer.add(x).add(y + 0.5f).add(z).add(color);
    buffer.add(x + 1.0f).add(y + 0.5f).add(z).add(color);
}