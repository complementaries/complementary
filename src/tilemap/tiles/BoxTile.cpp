#include "BoxTile.h"

BoxTile::BoxTile(Color color, bool solid) : Tile(color, solid, "default") {
}

void BoxTile::render(Buffer& buffer, float x, float y) const {
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

void BoxTile::renderEditor(Buffer& buffer, float x, float y, float z) const {
    float minX = x;
    float minY = y;
    float maxX = minX + 1;
    float maxY = minY + 1;
    Color color = getColor();
    buffer.add(minX).add(minY).add(z).add(color);
    buffer.add(maxX).add(minY).add(z).add(color);
    buffer.add(minX).add(maxY).add(z).add(color);
    buffer.add(maxX).add(maxY).add(z).add(color);
    buffer.add(maxX).add(minY).add(z).add(color);
    buffer.add(minX).add(maxY).add(z).add(color);
}