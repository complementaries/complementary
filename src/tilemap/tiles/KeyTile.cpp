#include "KeyTile.h"

#include "sound/SoundManager.h"
#include "player/Player.h"
#include "tilemap/Tilemap.h"
#include "tilemap/Tiles.h"

KeyTile::KeyTile() : Tile(ColorUtils::GRAY, false, "key") {
}

void KeyTile::onCollision(int x, int y) const {
    Tilemap::setTile(x, y, Tiles::COLLECTED_KEY);
    SoundManager::playSoundEffect(Sound::COLLECT);
}

void KeyTile::render(Buffer& buffer, float x, float y) const {
    Color color = getColor();
    buffer.add(x + 0.5f).add(y).add(color);
    buffer.add(x).add(y + 0.5f).add(color);
    buffer.add(x + 1.0f).add(y + 0.5f).add(color);
    buffer.add(x + 0.5f).add(y + 1.0f).add(color);
    buffer.add(x).add(y + 0.5f).add(color);
    buffer.add(x + 1.0f).add(y + 0.5f).add(color);
}

void KeyTile::renderEditor(Buffer& buffer, float x, float y, float z) const {
    Color color = getColor();
    buffer.add(x + 0.5f).add(y).add(z).add(color);
    buffer.add(x).add(y + 0.5f).add(z).add(color);
    buffer.add(x + 1.0f).add(y + 0.5f).add(z).add(color);
    buffer.add(x + 0.5f).add(y + 1.0f).add(z).add(color);
    buffer.add(x).add(y + 0.5f).add(z).add(color);
    buffer.add(x + 1.0f).add(y + 0.5f).add(z).add(color);
}