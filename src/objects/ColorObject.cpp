#include "ColorObject.h"

#include "ObjectRenderer.h"
#include "Objects.h"
#include "player/Player.h"
#include <memory>

ColorObject::ColorObject(ColorObjectData data) {
    this->data = data;
}

ColorObject::ColorObject(const Vector& position, const Vector& size, Ability a, Ability b) {
    this->position = position;
    data.size = size;
    data.abilities[0] = a;
    data.abilities[1] = b;
}

bool ColorObject::isSolid() const {
    return true;
}

void ColorObject::postInit() {
    particles =
        Objects::instantiateObject<ParticleSystem>("assets/particlesystems/colorblock.cmob");
}

void ColorObject::onFaceCollision(Face playerFace) {
    (void)playerFace;
    Player::setAbilities(data.abilities[0], data.abilities[1]);
}

bool ColorObject::collidesWith(const Vector& pPosition, const Vector& pSize) const {
    return position[0] < pPosition[0] + pSize[0] && position[0] + data.size[0] > pPosition[0] &&
           position[1] < pPosition[1] + pSize[1] && position[1] + data.size[1] > pPosition[1];
}

void ColorObject::tick() {
    particles->play();
    Color color = AbilityUtils::getColor(data.abilities[Player::invertColors()]);
    Vector size = this->getSize();
    particles->position = position;
    particles->data.boxSize = size * 1.1f;
    particles->position = position + size / 2.0f;
    particles->data.startColor = color;
    particles->data.endColor = ColorUtils::setAlpha(color, 150);
    particles->data.minStartVelocity = -size * 0.01f;
    particles->data.maxStartVelocity = size * 0.01f;
}

void ColorObject::render(float lag) {
    (void)lag;
    ObjectRenderer::drawRectangle(position, data.size,
                                  AbilityUtils::getColor(data.abilities[Player::invertColors()]));
}

std::shared_ptr<ObjectBase> ColorObject::clone() {
    return std::make_shared<ColorObject>(data);
}

#ifndef NDEBUG
void ColorObject::initTileEditorData(std::vector<TileEditorProp>& props) {
    props.insert(props.end(),
                 {
                     TileEditorProp::Int("Size X", data.size.x, 1, 5),
                     TileEditorProp::Int("Size Y", data.size.y, 1, 5),
                     TileEditorProp::Int("Ability 1", static_cast<int>(data.abilities[0]), 0.f,
                                         static_cast<int>(Ability::MAX) - 1),
                     TileEditorProp::Int("Ability 2", static_cast<int>(data.abilities[1]), 0.f,
                                         static_cast<int>(Ability::MAX) - 1),
                 });
}

void ColorObject::applyTileEditorData(float* props) {
    data.size.x = props[0];
    data.size.y = props[1];
    data.abilities[0] = static_cast<Ability>(props[2]);
    data.abilities[1] = static_cast<Ability>(props[3]);
}
#endif

Vector ColorObject::getSize() const {
    return data.size;
}