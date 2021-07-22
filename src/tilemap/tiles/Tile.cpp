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

char Tile::getId() const {
    return id;
}

void Tile::onFaceCollision(Face playerFace) const {
    (void)playerFace;
}

void Tile::onCollision(int x, int y) const {
    (void)x;
    (void)y;
}

bool Tile::isSolid() const {
    return solid;
}

bool Tile::isWall() const {
    return true;
}

void Tile::render(Buffer& buffer, float x, float y) const {
    (void)buffer;
    (void)x;
    (void)y;
}

bool Tile::operator==(const Tile& other) const {
    return this == &other;
}