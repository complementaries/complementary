#include "SpikeTile.h"

#include "player/Player.h"

SpikeTile::SpikeTile(Face face) : Tile(ColorUtils::BLACK, true, "default"), face(face) {
}

bool SpikeTile::isWall() const {
    return false;
}

void SpikeTile::onFaceCollision(Face playerFace) const {
    if (playerFace == FaceUtils::invert(face)) {
        Player::kill();
    }
}

void SpikeTile::render(Buffer& buffer, float x, float y, float z) const {
    Color color = getColor();
    switch (face) {
        default:
        case Face::UP:
            buffer.add(x).add(y + 0.33f).add(z).add(color);
            buffer.add(x + 0.165f).add(y).add(z).add(color);
            buffer.add(x + 0.33f).add(y + 0.33f).add(z).add(color);
            buffer.add(x + 0.33f).add(y + 0.33f).add(z).add(color);
            buffer.add(x + 0.5f).add(y).add(z).add(color);
            buffer.add(x + 0.66f).add(y + 0.33f).add(z).add(color);
            buffer.add(x + 0.66f).add(y + 0.33f).add(z).add(color);
            buffer.add(x + 0.825f).add(y).add(z).add(color);
            buffer.add(x + 1.0f).add(y + 0.33f).add(z).add(color);
            buffer.add(x).add(y + 0.33f).add(z).add(color);
            buffer.add(x + 1.0f).add(y + 1.0f).add(z).add(color);
            buffer.add(x + 1.0f).add(y + 0.33f).add(z).add(color);
            buffer.add(x).add(y + 0.33f).add(z).add(color);
            buffer.add(x + 1.0f).add(y + 1.0f).add(z).add(color);
            buffer.add(x).add(y + 1.0f).add(z).add(color);
            break;
        case Face::DOWN:
            buffer.add(x).add(y + 0.67f).add(z).add(color);
            buffer.add(x + 0.165f).add(y + 1.0f).add(z).add(color);
            buffer.add(x + 0.33f).add(y + 0.67f).add(z).add(color);
            buffer.add(x + 0.33f).add(y + 0.67f).add(z).add(color);
            buffer.add(x + 0.5f).add(y + 1.0f).add(z).add(color);
            buffer.add(x + 0.66f).add(y + 0.67f).add(z).add(color);
            buffer.add(x + 0.66f).add(y + 0.67f).add(z).add(color);
            buffer.add(x + 0.825f).add(y + 1.0f).add(z).add(color);
            buffer.add(x + 1.0f).add(y + 0.67f).add(z).add(color);
            buffer.add(x).add(y + 0.67f).add(z).add(color);
            buffer.add(x + 1.0f).add(y).add(z).add(color);
            buffer.add(x + 1.0f).add(y + 0.67f).add(z).add(color);
            buffer.add(x).add(y + 0.67f).add(z).add(color);
            buffer.add(x + 1.0f).add(y).add(z).add(color);
            buffer.add(x).add(y).add(z).add(color);
            break;
        case Face::LEFT:
            buffer.add(x + 0.33f).add(y).add(z).add(color);
            buffer.add(x).add(y + 0.165f).add(z).add(color);
            buffer.add(x + 0.33f).add(y + 0.33f).add(z).add(color);
            buffer.add(x + 0.33f).add(y + 0.33f).add(z).add(color);
            buffer.add(x).add(y + 0.5f).add(z).add(color);
            buffer.add(x + 0.33f).add(y + 0.66f).add(z).add(color);
            buffer.add(x + 0.33f).add(y + 0.66f).add(z).add(color);
            buffer.add(x).add(y + 0.825f).add(z).add(color);
            buffer.add(x + 0.33f).add(y + 1.0f).add(z).add(color);
            buffer.add(x + 0.33f).add(y).add(z).add(color);
            buffer.add(x + 1.0f).add(y + 1.0f).add(z).add(color);
            buffer.add(x + 0.33f).add(y + 1.0f).add(z).add(color);
            buffer.add(x + 0.33f).add(y).add(z).add(color);
            buffer.add(x + 1.0f).add(y + 1.0f).add(z).add(color);
            buffer.add(x + 1.0f).add(y).add(z).add(color);
            break;
        case Face::RIGHT:
            buffer.add(x + 0.67f).add(y).add(z).add(color);
            buffer.add(x + 1.0f).add(y + 0.165f).add(z).add(color);
            buffer.add(x + 0.67f).add(y + 0.33f).add(z).add(color);
            buffer.add(x + 0.67f).add(y + 0.33f).add(z).add(color);
            buffer.add(x + 1.0f).add(y + 0.5f).add(z).add(color);
            buffer.add(x + 0.67f).add(y + 0.66f).add(z).add(color);
            buffer.add(x + 0.67f).add(y + 0.66f).add(z).add(color);
            buffer.add(x + 1.0f).add(y + 0.825f).add(z).add(color);
            buffer.add(x + 0.67f).add(y + 1.0f).add(z).add(color);
            buffer.add(x + 0.67f).add(y).add(z).add(color);
            buffer.add(x).add(y + 1.0f).add(z).add(color);
            buffer.add(x + 0.67f).add(y + 1.0f).add(z).add(color);
            buffer.add(x + 0.67f).add(y).add(z).add(color);
            buffer.add(x).add(y + 1.0f).add(z).add(color);
            buffer.add(x).add(y).add(z).add(color);
            break;
    }
}