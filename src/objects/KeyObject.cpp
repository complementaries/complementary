#include "KeyObject.h"

#include "ObjectRenderer.h"
#include "objects/Objects.h"
#include "player/Player.h"
#include "sound/SoundManager.h"

#include <cmath>
#include <memory>

KeyObject::KeyObject() : KeyObject(Vector(), 0) {
}

KeyObject::KeyObject(KeyObjectData data) : KeyObject(Vector(), data.type) {
}

KeyObject::KeyObject(const Vector& position, int type)
    : counter(0), alpha(255), collected(false), added(false) {
    this->position = position;
    data.type = type;
    lastRenderPosition = position;
    renderPosition = position;
}

void KeyObject::postInit() {
    lastRenderPosition = position;
    renderPosition = position;
}

void KeyObject::onCollision() {
    collected = true;
    SoundManager::playSoundEffect(Sound::COLLECT);

    std::shared_ptr<ObjectBase> door = Objects::findDoor(data.type);
    if (door == nullptr) {
        goal = position;
    } else {
        goal = door->position + door->getSize() * 0.5f - Vector(0.5f, 0.5f);
    }
}

bool KeyObject::collidesWith(const Vector& pPosition, const Vector& pSize) const {
    if (collected) {
        return false;
    }
    return position[0] < pPosition[0] + pSize[0] && position[0] + 1.0f > pPosition[0] &&
           position[1] < pPosition[1] + pSize[1] && position[1] + 1.0f > pPosition[1];
}

void KeyObject::tick() {
    counter = (counter + 1) % 150;

    lastRenderPosition = renderPosition;
    if (!collected) {
        renderPosition = position;
        return;
    } else if (added) {
        alpha -= 3;
        if (alpha < 0) {
            alpha = 0;
        }
        return;
    }
    Vector diff = goal - renderPosition;
    float length = diff.getLength();
    float speed = 0.25f;
    if (length > speed) {
        renderPosition += diff * speed / length;
    } else {
        renderPosition = goal;
        added = true;
        std::shared_ptr<ObjectBase> door = Objects::findDoor(data.type);
        if (door != nullptr) {
            door->addKey();
        }
    }
}

void KeyObject::renderColor(float lag, Color color) {
    color = ColorUtils::setAlpha(color, alpha);
    Vector pos = lastRenderPosition + (renderPosition - lastRenderPosition) * lag +
                 Vector(0.0f, sinf((counter + lag) * (6.283185307f / 150.0f)) * 0.125f);

    Vector a = pos + Vector(0.0f, 0.5f);
    Vector b = pos + Vector(0.5f, 0.0f);
    Vector c = pos + Vector(1.0f, 0.5f);
    ObjectRenderer::drawTriangle(a, b, c, color);
    b = pos + Vector(0.5f, 1.0f);
    ObjectRenderer::drawTriangle(a, b, c, color);

    constexpr float oversize = 1.0f;
    int base = 255 * Player::invertColors();
    Color borderColor = ColorUtils::rgba(base, base, base, 0);
    Color midColor = ColorUtils::rgba(base, base, base, alpha);
    a = pos + Vector(-oversize, 0.5f);
    b = pos + Vector(0.5f, -oversize);
    c = pos + Vector(0.5f, 0.5f);
    ObjectRenderer::drawTriangle(a, b, c, borderColor, borderColor, midColor);
    a = pos + Vector(1.0f + oversize, 0.5f);
    ObjectRenderer::drawTriangle(a, b, c, borderColor, borderColor, midColor);
    b = pos + Vector(0.5f, 1.0f + oversize);
    ObjectRenderer::drawTriangle(a, b, c, borderColor, borderColor, midColor);
    a = pos + Vector(-oversize, 0.5f);
    ObjectRenderer::drawTriangle(a, b, c, borderColor, borderColor, midColor);
}

void KeyObject::lateRender(float lag) {
    renderColor(lag, ColorUtils::GRAY);
}

void KeyObject::renderEditor(float lag) {
    lastRenderPosition = position;
    renderPosition = position;
    constexpr Color colors[] = {ColorUtils::RED, ColorUtils::GREEN, ColorUtils::BLUE};
    renderColor(lag, colors[(data.type & 0x3) % 3]);
}

std::shared_ptr<ObjectBase> KeyObject::clone() {
    return std::make_shared<KeyObject>(data);
}

#ifndef NDEBUG
void KeyObject::initTileEditorData(std::vector<TileEditorProp>& props) {
    props.insert(props.end(), {
                                  TileEditorProp::Int("Type", data.type, 0, 2),
                              });
}

void KeyObject::applyTileEditorData(float* props) {
    data.type = props[0];
}
#endif

Vector KeyObject::getSize() const {
    return Vector(1.0f, 1.0f);
}

void KeyObject::reset() {
    collected = false;
    added = false;
    alpha = 255;
    lastRenderPosition = position;
    renderPosition = position;
    goal = Vector();
}

bool KeyObject::isKeyOfType(int type) const {
    return data.type == type;
}