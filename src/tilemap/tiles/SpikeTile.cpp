#include "SpikeTile.h"

#include "player/Player.h"
#include "tilemap/Tilemap.h"

SpikeTile::SpikeTile(Face face) : Tile(ColorUtils::BLACK, true, "default"), face(face) {
}

bool SpikeTile::isWall() const {
    return false;
}

void SpikeTile::onFaceCollision(Face playerFace) const {
    if (face == Face::MAX) {
        Player::kill();
    } else if (playerFace == FaceUtils::invert(face)) {
        Player::kill();
    }
}

void SpikeTile::render(Buffer& buffer, float x, float y, float z) const {
    Color c = getColor();
    switch (face) {
        case Face::UP: addSpike(buffer, x, y, z, false, false, true, false, c); break;
        case Face::DOWN: addSpike(buffer, x, y, z, false, false, false, true, c); break;
        case Face::LEFT: addSpike(buffer, x, y, z, true, false, false, false, c); break;
        case Face::RIGHT: addSpike(buffer, x, y, z, false, true, false, false, c); break;
        default:
            addSpike(buffer, x, y, z, Tilemap::getTile(x - 1, y).getId() <= 0,
                     Tilemap::getTile(x + 1, y).getId() <= 0,
                     Tilemap::getTile(x, y - 1).getId() <= 0,
                     Tilemap::getTile(x, y + 1).getId() <= 0, c);
            break;
    }
}

void SpikeTile::renderEditor(Buffer& buffer, float x, float y, float z) const {
    if (face == Face::MAX) {
        addSpike(buffer, x, y, z, true, true, true, true, getColor());
    } else {
        render(buffer, x, y, z);
    }
}

static void addRectangle(Buffer& buffer, float x, float y, float z, float w, float h, Color c) {
    buffer.add(x).add(y).add(z).add(c);
    buffer.add(x + w).add(y + h).add(z).add(c);
    buffer.add(x + w).add(y).add(z).add(c);
    buffer.add(x).add(y).add(z).add(c);
    buffer.add(x + w).add(y + h).add(z).add(c);
    buffer.add(x).add(y + h).add(z).add(c);
}

void SpikeTile::addSpike(Buffer& buffer, float x, float y, float z, bool left, bool right, bool up,
                         bool down, Color c) {
    constexpr float s = 0.1f;
    constexpr float ss = 0.7f;
    if (left && !up) {
        buffer.add(x + 0.5f - s).add(y).add(z).add(c);
        buffer.add(x).add(y + 0.25f).add(z).add(c);
        buffer.add(x + 0.5f - s).add(y + 0.5f).add(z).add(c);
        addRectangle(buffer, x + 0.5f - s, y, z, s, 0.5f, c);
    } else if (!left && up) {
        buffer.add(x).add(y + 0.5f - s).add(z).add(c);
        buffer.add(x + 0.25f).add(y).add(z).add(c);
        buffer.add(x + 0.5f).add(y + 0.5f - s).add(z).add(c);
        addRectangle(buffer, x, y + 0.5f - s, z, 0.5f, s, c);
    } else if (left && up) {
        buffer.add(x).add(y).add(z).add(c);
        buffer.add(x + ss).add(y + 0.5f - s).add(z).add(c);
        buffer.add(x + 0.5f - s).add(y + ss).add(z).add(c);
    } else {
        addRectangle(buffer, x, y, z, 0.5f, 0.5f, c);
    }

    if (right && !up) {
        buffer.add(x + 0.5f + s).add(y).add(z).add(c);
        buffer.add(x + 1.0f).add(y + 0.25f).add(z).add(c);
        buffer.add(x + 0.5f + s).add(y + 0.5f).add(z).add(c);
        addRectangle(buffer, x + 0.5f, y, z, s, 0.5f, c);
    } else if (!right && up) {
        buffer.add(x + 0.5f).add(y + 0.5f - s).add(z).add(c);
        buffer.add(x + 0.75f).add(y).add(z).add(c);
        buffer.add(x + 1.0f).add(y + 0.5f - s).add(z).add(c);
        addRectangle(buffer, x + 0.5f, y + 0.5f - s, z, 0.5f, s, c);
    } else if (right && up) {
        buffer.add(x + 1.0f).add(y).add(z).add(c);
        buffer.add(x + 1.0f - ss).add(y + 0.5f - s).add(z).add(c);
        buffer.add(x + 0.5f + s).add(y + ss).add(z).add(c);
    } else {
        addRectangle(buffer, x + 0.5f, y, z, 0.5f, 0.5f, c);
    }

    if (left && !down) {
        buffer.add(x + 0.5f - s).add(y + 0.5f).add(z).add(c);
        buffer.add(x).add(y + 0.75f).add(z).add(c);
        buffer.add(x + 0.5f - s).add(y + 1.0f).add(z).add(c);
        addRectangle(buffer, x + 0.5f - s, y + 0.5f, z, s, 0.5f, c);
    } else if (!left && down) {
        buffer.add(x).add(y + 0.5f + s).add(z).add(c);
        buffer.add(x + 0.25f).add(y + 1.0f).add(z).add(c);
        buffer.add(x + 0.5f).add(y + 0.5f + s).add(z).add(c);
        addRectangle(buffer, x, y + 0.5f, z, 0.5f, s, c);
    } else if (left && down) {
        buffer.add(x).add(y + 1.0f).add(z).add(c);
        buffer.add(x + 0.5f - s).add(y + 1.0f - ss).add(z).add(c);
        buffer.add(x + ss).add(y + 0.5f + s).add(z).add(c);
    } else {
        addRectangle(buffer, x, y + 0.5f, z, 0.5f, 0.5f, c);
    }

    if (right && !down) {
        buffer.add(x + 0.5f + s).add(y + 0.5f).add(z).add(c);
        buffer.add(x + 1.0f).add(y + 0.75f).add(z).add(c);
        buffer.add(x + 0.5f + s).add(y + 1.0f).add(z).add(c);
        addRectangle(buffer, x + 0.5f, y + 0.5f, z, s, 0.5f, c);
    } else if (!right && down) {
        buffer.add(x + 0.5f).add(y + 0.5f + s).add(z).add(c);
        buffer.add(x + 0.75f).add(y + 1.0f).add(z).add(c);
        buffer.add(x + 1.0f).add(y + 0.5f + s).add(z).add(c);
        addRectangle(buffer, x + 0.5f, y + 0.5f, z, 0.5f, s, c);
    } else if (right && down) {
        buffer.add(x + 1.0f).add(y + 1.0f).add(z).add(c);
        buffer.add(x + 0.5f - s).add(y + ss).add(z).add(c);
        buffer.add(x + 0.5f + s).add(y + 1.0f - ss).add(z).add(c);
    } else {
        addRectangle(buffer, x + 0.5f, y + 0.5f, z, 0.5f, 0.5f, c);
    }
}