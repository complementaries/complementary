#include "ColorObject.h"

#include "ObjectRenderer.h"
#include "player/Player.h"

ColorObject::ColorObject(const Vector& position, const Vector& size, Color c)
    : position(position), size(size), color(c) {
}

bool ColorObject::isSolid() const {
    return true;
}

void ColorObject::onFaceCollision(Face playerFace) {
    (void)playerFace;
    Player::setAbilities(Ability::WALL_JUMP, Ability::DASH);
}

bool ColorObject::collidesWith(const Vector& pPosition, const Vector& pSize) const {
    return position[0] < pPosition[0] + pSize[0] && position[0] + size[0] > pPosition[0] &&
           position[1] < pPosition[1] + pSize[1] && position[1] + size[1] > pPosition[1];
}

void ColorObject::render(float lag) const {
    (void)lag;
    ObjectRenderer::drawRectangle(position, size, color);
}