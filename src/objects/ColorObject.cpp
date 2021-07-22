#include "ColorObject.h"

#include "ObjectRenderer.h"
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

void ColorObject::onFaceCollision(Face playerFace) {
    (void)playerFace;
    Player::setAbilities(data.abilities[0], data.abilities[1]);
}

bool ColorObject::collidesWith(const Vector& pPosition, const Vector& pSize) const {
    return position[0] < pPosition[0] + pSize[0] && position[0] + data.size[0] > pPosition[0] &&
           position[1] < pPosition[1] + pSize[1] && position[1] + data.size[1] > pPosition[1];
}

void ColorObject::render(float lag) const {
    (void)lag;
    ObjectRenderer::drawRectangle(position, data.size,
                                  AbilityUtils::getColor(data.abilities[Player::invertColors()]));
}

std::shared_ptr<ObjectBase> ColorObject::clone() {
    return std::make_shared<ColorObject>(data);
}
