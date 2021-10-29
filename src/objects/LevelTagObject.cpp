#include "LevelTagObject.h"

#include "Game.h"
#include "ObjectRenderer.h"
#include "objects/Objects.h"
#include "player/Player.h"
#include "sound/SoundManager.h"

#include <cmath>
#include <memory>

LevelTagObject::LevelTagObject(Vector position, Vector size) {
    this->position = position;
    data = {};
    data.size = size;
}

LevelTagObject::LevelTagObject(LevelTagObjectData data) {
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
