#include "SpikeTile.h"

#include "player/Player.h"

SpikeTile::SpikeTile() : Tile(ColorUtils::GRAY, true, "default") {
}

bool SpikeTile::isWall() const {
    return false;
}

void SpikeTile::onFaceCollision(Face playerFace) const {
    if (playerFace == Face::DOWN) {
        Player::kill();
    }
}

void SpikeTile::render(Buffer& buffer, float x, float y) const {
    Color color = getColor();
    buffer.add(x + 0.5f).add(y).add(color);
    buffer.add(x + 1.0f).add(y + 1.0f).add(color);
    buffer.add(x).add(y + 1.0f).add(color);
}

void SpikeTile::renderEditor(Buffer& buffer, float x, float y, float z) const {
    Color color = getColor();
    buffer.add(x + 0.5f).add(y).add(z).add(color);
    buffer.add(x + 1.0f).add(y + 1.0f).add(z).add(color);
    buffer.add(x).add(y + 1.0f).add(z).add(color);
}