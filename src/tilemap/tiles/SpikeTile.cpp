#include "SpikeTile.h"

#include "player/Player.h"

SpikeTile::SpikeTile() : Tile(ColorUtils::rgba(0xFF, 0x00, 0x00), true) {
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