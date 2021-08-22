#include "WindObject.h"

#include "ObjectRenderer.h"
#include "player/Player.h"
#include <memory>

WindObject::WindObject() {
}

WindObject::WindObject(const WindObjectData& data) {
    this->data = data;
}

WindObject::WindObject(const Vector& position, const Vector& size, const Vector& force) {
    this->position = position;
    data.size = size;
    data.force = force;
}

void WindObject::onCollision() {
    if (Player::isGliding()) {
        Player::addForce(data.force);
    }
}

bool WindObject::collidesWith(const Vector& pPosition, const Vector& pSize) const {
    return position[0] < pPosition[0] + pSize[0] && position[0] + data.size[0] > pPosition[0] &&
           position[1] < pPosition[1] + pSize[1] && position[1] + data.size[1] > pPosition[1];
}

void WindObject::render(float lag) const {
    (void)lag;
    ObjectRenderer::drawRectangle(position, data.size, ColorUtils::rgba(255, 0, 0, 100));
}

std::shared_ptr<ObjectBase> WindObject::clone() {
    return std::make_shared<WindObject>(data);
}
