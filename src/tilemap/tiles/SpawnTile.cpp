#include "SpawnTile.h"

SpawnTile::SpawnTile() : Tile(ColorUtils::GRAY, false, "default") {
}

void SpawnTile::renderEditor(Buffer& buffer, float x, float y) const {
    float minX = x + 0.2f;
    float minY = y + 0.2f;
    float maxX = minX + 0.6f;
    float maxY = minY + 0.6f;
    Color color = getColor();
    buffer.add(minX).add(minY).add(color);
    buffer.add(maxX).add(minY).add(color);
    buffer.add(minX).add(maxY).add(color);
    buffer.add(maxX).add(maxY).add(color);
    buffer.add(maxX).add(minY).add(color);
    buffer.add(minX).add(maxY).add(color);
}

Color SpawnTile::getColor() const {
    return ColorUtils::rgba(255, 0, 255);
}
