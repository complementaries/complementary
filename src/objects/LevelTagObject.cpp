#include "LevelTagObject.h"

#include "Game.h"
#include "ObjectRenderer.h"
#include "Savegame.h"
#include "TextUtils.h"
#include "graphics/Font.h"
#include "graphics/gl/Glew.h"
#include "objects/Objects.h"
#include "player/Player.h"
#include "sound/SoundManager.h"

#include <cmath>
#include <memory>

LevelTagObject::LevelTagObject(Vector position, Vector size) : bestTimeAlpha(0.f) {
    this->position = position;
    data = {};
    data.size = size;
}

LevelTagObject::LevelTagObject(LevelTagObjectData data) : bestTimeAlpha(0.f) {
}

void LevelTagObject::tick() {
    bestTimeAlpha += Player::isAllowedToMove() && Player::isColliding(*this) ? 10.f : -10.f;
    bestTimeAlpha = std::max(std::min(bestTimeAlpha, 255), 0);
}

void LevelTagObject::onCollision() {
    Game::setNextLevelIndex(data.level);
}

bool LevelTagObject::collidesWith(const Vector& pPosition, const Vector& pSize) const {
    return position[0] < pPosition[0] + pSize[0] && position[0] + data.size[0] > pPosition[0] &&
           position[1] < pPosition[1] + pSize[1] && position[1] + data.size[1] > pPosition[1];
}

void LevelTagObject::renderEditor(float lag, bool inPalette) {
    (void)lag;
    (void)inPalette;
    ObjectRenderer::drawRectangle(position, data.size, ColorUtils::rgba(0, 100, 0, 100));
}

void LevelTagObject::renderText(float lag) {
    if (data.level > Savegame::getCompletedLevels()) {
        return;
    }
    constexpr float SIZE = 2.0f;
    char buffer[256];
    snprintf(buffer, 256, "%d", data.level + 1);
    float width = Font::getWidth(SIZE, buffer);
    Vector pos = position + data.size * 0.5f;
    pos.x -= width * 0.5f;
    pos.y -= SIZE * 0.5f;
    Color c =
        ColorUtils::setAlpha(Player::invertColors() ? ColorUtils::WHITE : ColorUtils::BLACK, 100);
    Font::draw(pos, SIZE, c, buffer);

    if (bestTimeAlpha > 0) {
        uint32_t completionTime = Savegame::getCompletionTime(data.level);
        if (completionTime > 0) {
            if (bestTimeAlpha < 150) {
                glDepthMask(false);
                TextUtils::drawBestTimeObjectSpace(Player::getPosition(), completionTime,
                                                   bestTimeAlpha);
                glDepthMask(true);
            } else {
                TextUtils::drawBestTimeObjectSpace(Player::getPosition(), completionTime,
                                                   bestTimeAlpha);
            }
        }
    }
}

std::shared_ptr<ObjectBase> LevelTagObject::clone() {
    return std::make_shared<LevelTagObject>(data);
}

#ifndef NDEBUG
void LevelTagObject::initTileEditorData(std::vector<TileEditorProp>& props) {
    props.insert(props.end(),
                 {TileEditorProp::Int("Size X", data.size.x, 0, 10),
                  TileEditorProp::Int("Size Y", data.size.y, 0, 10),
                  TileEditorProp::Float("Level", static_cast<float>(data.level), 0, 20)});
}

void LevelTagObject::applyTileEditorData(float* props) {
    data.size.x = props[0];
    data.size.y = props[1];
    data.level = static_cast<int>(props[2]);
}
#endif

Vector LevelTagObject::getSize() const {
    return data.size;
}
