#include "ColorObject.h"

#include "ObjectRenderer.h"
#include "player/Player.h"

ColorObject::ColorObject(const Vector& position, const Vector& size, Ability a, Ability b)
    : position(position), size(size) {
    abilities[0] = a;
    abilities[1] = b;
}

bool ColorObject::isSolid() const {
    return true;
}

void ColorObject::onFaceCollision(Face playerFace) {
    (void)playerFace;
    Player::setAbilities(abilities[0], abilities[1]);
}

bool ColorObject::collidesWith(const Vector& pPosition, const Vector& pSize) const {
    return position[0] < pPosition[0] + pSize[0] && position[0] + size[0] > pPosition[0] &&
           position[1] < pPosition[1] + pSize[1] && position[1] + size[1] > pPosition[1];
}

void ColorObject::render(float lag) const {
    (void)lag;
    ObjectRenderer::drawRectangle(position, size,
                                  AbilityUtils::getColor(abilities[Player::invertColors()]));
}