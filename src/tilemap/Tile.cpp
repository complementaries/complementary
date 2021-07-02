#include "Tile.h"

Tile::Tile(Color color, bool solid) : id(-1), color(color), solid(solid) {
}

Color Tile::getColor() const {
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