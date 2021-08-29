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

    // TEMP: indicator for the wind direction
    auto normForce = data.force;
    normForce.normalize();
    ObjectRenderer::drawRectangle(position + data.size * 0.5f, Vector(1.f, 1.f),
                                  ColorUtils::rgba(255, 0, 0, 200));
    ObjectRenderer::drawRectangle(position + data.size * 0.5f + normForce + Vector(0.2f, 0.2f),
                                  Vector(0.4f, 0.4f), ColorUtils::rgba(255, 0, 0));
}

std::shared_ptr<ObjectBase> WindObject::clone() {
    return std::make_shared<WindObject>(data);
}

#ifndef NDEBUG
void WindObject::initTileEditorData(std::vector<TileEditorProp>& props) {
    props.insert(props.end(), {
                                  TileEditorProp::Int("Size X", data.size.x, 0, 5),
                                  TileEditorProp::Int("Size Y", data.size.y, 0, 5),
                                  TileEditorProp::Float("Force X", data.force.x, 0.f, 0.5f, 0.01f),
                                  TileEditorProp::Float("Force Y", data.force.y, 0.f, 0.5f, 0.01f),
                              });
}

void WindObject::applyTileEditorData(float* props) {
    data.size.x = props[0];
    data.size.y = props[1];
    data.force.x = props[2];
    data.force.y = props[3];
}
#endif
