#include "DoorObject.h"

#include "ObjectRenderer.h"
#include "objects/Objects.h"
#include "player/Player.h"

#include <memory>

constexpr int START_ALPHA = 400;

DoorObject::DoorObject() : DoorObject(Vector(), Vector(1.0f, 1.0f), 0) {
}

DoorObject::DoorObject(DoorObjectData data) : DoorObject(Vector(), data.size, data.type) {
}

DoorObject::DoorObject(const Vector& position, const Vector& size, int type)
    : maxKeys(0), keys(0), alpha(START_ALPHA) {
    this->position = position;
    data.size = size;
    data.type = type;
}

bool DoorObject::isSolid() const {
    return alpha > 150;
}

bool DoorObject::collidesWith(const Vector& pPosition, const Vector& pSize) const {
    return position[0] < pPosition[0] + pSize[0] && position[0] + data.size[0] > pPosition[0] &&
           position[1] < pPosition[1] + pSize[1] && position[1] + data.size[1] > pPosition[1];
}

void DoorObject::tick() {
    alpha -= (keys == maxKeys && maxKeys != 0) * 3;
    if (alpha < 0) {
        alpha = 0;
    }
}

void DoorObject::render(float lag, Color color) {
    (void)lag;
    color = ColorUtils::setAlpha(color, std::min(alpha, 255));
    ObjectRenderer::drawRectangle(position, data.size, color);
    if (alpha < 255) {
        return;
    }

    Color keyholeColor;
    Color background = Player::invertColors() ? ColorUtils::BLACK : ColorUtils::WHITE;
    if (maxKeys != 0) {
        keyholeColor = ColorUtils::mix(background, color, static_cast<float>(keys) / maxKeys);
    } else {
        keyholeColor = background;
    }
    Vector mid = position + data.size * 0.5f;
    Vector a = mid + Vector(-0.4f, 0.0f);
    Vector b = mid + Vector(0.4f, 0.0f);
    Vector c = mid + Vector(0.0f, -0.4f);
    ObjectRenderer::drawTriangle(a, b, c, keyholeColor);
    c = mid + Vector(0.0f, 0.4f);
    ObjectRenderer::drawTriangle(a, b, c, keyholeColor);
}

void DoorObject::render(float lag) {
    constexpr Color colors[] = {ColorUtils::DARK_GRAY, ColorUtils::LIGHT_GRAY};
    render(lag, colors[Player::invertColors()]);
}

void DoorObject::renderEditor(float lag) {
    constexpr Color colors[] = {ColorUtils::RED, ColorUtils::GREEN, ColorUtils::BLUE};
    render(lag, colors[(data.type & 0x3) % 3]);
}

std::shared_ptr<ObjectBase> DoorObject::clone() {
    return std::make_shared<DoorObject>(data);
}

#ifndef NDEBUG
void DoorObject::initTileEditorData(std::vector<TileEditorProp>& props) {
    props.insert(props.end(), {TileEditorProp::Int("Size X", data.size.x, 1, 5),
                               TileEditorProp::Int("Size Y", data.size.y, 1, 5),
                               TileEditorProp::Int("Type", data.type, 0, 2)});
}

void DoorObject::applyTileEditorData(float* props) {
    data.size.x = props[0];
    data.size.y = props[1];
    data.type = props[2];
}
#endif

Vector DoorObject::getSize() const {
    return data.size;
}

bool DoorObject::isDoorOfType(int type) const {
    return data.type == type;
}

void DoorObject::addKey() {
    keys++;
    maxKeys = Objects::countKeys(data.type);
}

void DoorObject::reset() {
    keys = 0;
    maxKeys = 0;
    alpha = START_ALPHA;
}
