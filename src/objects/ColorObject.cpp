#include "ColorObject.h"

#include <memory>

#include "ObjectRenderer.h"
#include "Objects.h"
#include "Savegame.h"
#include "player/Player.h"
#include "sound/SoundManager.h"

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
    particles->play();

    Vector size = this->getSize();
    particles->position = position;
    particles->data.boxSize = size;
    particles->position = position + size / 2.0f;
    particles->data.minEmissionRate = static_cast<int>((size.x + size.y) / 2.0f * 5.0f);
}

void ColorObject::onFaceCollision(Face playerFace) {
    Ability a = Player::getAbility();
    Player::setAbilities(data.abilities[0], data.abilities[1],
                         Savegame::abilitiesUnlocked(data.abilities[0], data.abilities[1]) ||
                             data.abilities[0] == Ability::NONE ||
                             data.abilities[1] == Ability::NONE);
    Ability b = Player::getAbility();
    if (a != b) {
        SoundManager::playSoundEffect(b == Ability::NONE ? Sound::NO_ABILITY : Sound::ABILITY);
    }
}

bool ColorObject::collidesWith(const Vector& pPosition, const Vector& pSize) const {
    return position[0] < pPosition[0] + pSize[0] && position[0] + data.size[0] > pPosition[0] &&
           position[1] < pPosition[1] + pSize[1] && position[1] + data.size[1] > pPosition[1];
}

void ColorObject::tick() {
    Color color = AbilityUtils::getColor(data.abilities[Player::invertColors()]);
    particles->data.startColor = color;
    particles->data.endColor = ColorUtils::setAlpha(color, 150);
}

void ColorObject::render(float lag) {
    (void)lag;
    ObjectRenderer::addRectangle(position, data.size,
                                 AbilityUtils::getColor(data.abilities[Player::invertColors()]));
}

std::shared_ptr<ObjectBase> ColorObject::clone() {
    return std::make_shared<ColorObject>(data);
}

#ifndef NDEBUG
void ColorObject::initTileEditorData(std::vector<TileEditorProp>& props) {
    props.insert(props.end(),
                 {
                     TileEditorProp::Int("Size X", data.size.x, 1, 20),
                     TileEditorProp::Int("Size Y", data.size.y, 1, 10),
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

bool ColorObject::hasMoved() const {
    return false;
}