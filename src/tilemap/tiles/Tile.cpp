#include "Tile.h"
#include "player/Player.h"

Tile::Tile(Color color, bool solid) : id(-1), color(color), solid(solid) {
}

Color Tile::getColor() const {
    if (Player::invertColors()) {
        return ColorUtils::invert(color);
    }
    return color;
}

int Tile::getId() const {
    return id;
}

void Tile::onFaceCollision(Face playerFace) const {
    (void)playerFace;
}

void Tile::onCollision() const {
}

bool Tile::isSolid() const {
    return solid;
}

void Tile::render(Buffer& buffer, float x, float y) const {
    float minX = x;
    float minY = y;
    float maxX = minX + 1;
    float maxY = minY + 1;
    Color color = getColor();
    buffer.add(minX).add(minY).add(color);
    buffer.add(maxX).add(minY).add(color);
    buffer.add(minX).add(maxY).add(color);
    buffer.add(maxX).add(maxY).add(color);
    buffer.add(maxX).add(minY).add(color);
    buffer.add(minX).add(maxY).add(color);
}