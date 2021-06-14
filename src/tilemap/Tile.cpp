#include "Tile.h"

Tile::Tile(Color color) : id(-1), color(color) {
}

Color Tile::getColor() const {
    return color;
}

int Tile::getId() const {
    return id;
}