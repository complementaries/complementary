#include "MovingObject.h"

#include "ObjectRenderer.h"
#include "player/Player.h"
#include <iostream>
#include <memory>

MovingObject::MovingObject() {
}

MovingObject::MovingObject(const MovingObjectData& data) {
    this->data = data;
}

MovingObject::MovingObject(const Vector& size, const Vector& a, const Vector& b, float speed) {
    this->position = a;
    lastPosition = a;
    data.size = size;
    data.goalA = a;
    data.goalB = b;
    data.speed = speed;
}

void MovingObject::tick() {
    lastPosition = position;

    velocity = data.goalB - position;
    float length = velocity.getLength();
    if (length < data.speed) {
        position = data.goalB;
        std::swap(data.goalA, data.goalB);
    } else {
        velocity *= data.speed / length;
        position += velocity;
    }

    while (Player::isColliding(*this)) {
        Vector step = velocity;
        if (step.y != 0.0f) {
            step.x = 0.0f;
        }
        step.normalize();
        step *= 0.01f;
        Player::moveForced(step);
    }
}

bool MovingObject::isSolid() const {
    return true;
}

void MovingObject::onFaceCollision(Face playerFace) {
    if (playerFace == Face::DOWN) {
        Player::addBaseVelocity(velocity);
    }
}

bool MovingObject::collidesWith(const Vector& pPosition, const Vector& pSize) const {
    return position[0] < pPosition[0] + pSize[0] && position[0] + data.size[0] > pPosition[0] &&
           position[1] < pPosition[1] + pSize[1] && position[1] + data.size[1] > pPosition[1];
}

void MovingObject::render(float lag) const {
    Vector oversize(0.05f, 0.05f);
    Vector i = lastPosition + (position - lastPosition) * lag;
    ObjectRenderer::drawRectangle(i - oversize, data.size + oversize * 2.0f, ColorUtils::GRAY);
}

std::shared_ptr<ObjectBase> MovingObject::clone() {
    return std::make_shared<MovingObject>(data);
}

#ifndef NDEBUG
void MovingObject::initTileEditorData(std::vector<TileEditorProp>& props) {
    props.insert(props.end(), {
                                  TileEditorProp::Int("Size X", data.size.x, 0, 5),
                                  TileEditorProp::Int("Size Y", data.size.y, 0, 5),
                                  TileEditorProp::Float("Goal 1 X", data.goalA.x, 0.f, 200.f),
                                  TileEditorProp::Float("Goal 1 Y", data.goalA.y, 0.f, 200.f),
                                  TileEditorProp::Float("Goal 2 X", data.goalB.x, 0.f, 200.f),
                                  TileEditorProp::Float("Goal 2 Y", data.goalB.y, 0.f, 200.f),
                                  TileEditorProp::Float("Speed", data.speed, 0.f, 0.5f),
                              });
}

void MovingObject::applyTileEditorData(float* props) {
    data.size.x = props[0];
    data.size.y = props[1];
    data.goalA.x = props[2];
    data.goalA.y = props[3];
    data.goalB.x = props[4];
    data.goalB.y = props[5];
    data.speed = props[6];
}
#endif
