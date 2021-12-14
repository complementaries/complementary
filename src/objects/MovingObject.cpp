#include "MovingObject.h"

#include <cmath>

#include "ObjectRenderer.h"
#include "objects/Objects.h"
#include "player/Player.h"
#include <iostream>

MovingObject::MovingObject() {
}

MovingObject::MovingObject(const MovingObjectData& data) : movingBack(false) {
    this->data = data;
}

MovingObject::MovingObject(const Vector& size, const Vector& goal, float speed)
    : movingBack(false) {
    data.size = size;
    data.goal = goal;
    data.speed = speed;
    data.spiky[0] = true;
    data.spiky[1] = false;
    data.spiky[2] = false;
    data.spiky[3] = false;
}

void MovingObject::postInit() {
    initialPosition = position;
    lastPosition = position;
}

void MovingObject::reset() {
    position = initialPosition;
    lastPosition = position;
}

void MovingObject::tick() {
    lastPosition = position;

    auto goal = movingBack ? initialPosition : initialPosition + data.goal;
    velocity = goal - position;
    float length = velocity.getLength();
    if (length < 0.0005f || data.speed < 0.0005f) {
        velocity = Vector();
        movingBack = !movingBack;
        return;
    }
    if (length < data.speed) {
        position = goal;
        movingBack = !movingBack;
    } else {
        velocity *= data.speed / length;
        position += velocity;
    }

    Vector step = velocity;
    if (step.y != 0.0f) {
        step.x = 0.0f;
    }
    step.normalize();
    step *= 0.01f;
    while (Player::isColliding(*this)) {
        if ((step.y > 0.0f && data.spiky[static_cast<int>(Face::DOWN)]) ||
            (step.y < 0.0f && data.spiky[static_cast<int>(Face::UP)]) ||
            (step.x < 0.0f && data.spiky[static_cast<int>(Face::LEFT)]) ||
            (step.x > 0.0f && data.spiky[static_cast<int>(Face::RIGHT)])) {
            Player::kill();
            break;
        }
        Player::moveForced(step);
    }

    Vector grow(0.0f, 0.1f);
    Objects::forceMoveParticles(position - grow, data.size + grow, velocity);
}

bool MovingObject::isSolid() const {
    return true;
}

void MovingObject::onFaceCollision(Face playerFace) {
    if (playerFace != Face::UP) {
        Player::addBaseVelocity(velocity);
    }
    if (data.spiky[static_cast<int>(FaceUtils::invert(playerFace))]) {
        Player::kill();
    }
}

bool MovingObject::collidesWith(const Vector& pPosition, const Vector& pSize) const {
    return position[0] < pPosition[0] + pSize[0] && position[0] + data.size[0] > pPosition[0] &&
           position[1] < pPosition[1] + pSize[1] && position[1] + data.size[1] > pPosition[1];
}

bool MovingObject::isSpiky(Face f) const {
    return data.spiky[static_cast<int>(f)];
}

void MovingObject::renderAt(float lag, Color color, Vector p) {
    int endX = data.size.x - 1;
    int endY = data.size.y - 1;
    for (int x = 0; x <= endX; x++) {
        for (int y = 0; y <= endY; y++) {
            ObjectRenderer::addSpike(position + Vector(x, y), x == 0 && isSpiky(Face::LEFT),
                                     x == endX && isSpiky(Face::RIGHT), y == 0 && isSpiky(Face::UP),
                                     y == endY && isSpiky(Face::DOWN), color);
        }
    }
}

void MovingObject::render(float lag, Color color) {
    renderAt(lag, color, lastPosition + (position - lastPosition) * lag);
}

void MovingObject::render(float lag) {
    render(lag, Player::invertColors() ? ColorUtils::WHITE : ColorUtils::BLACK);
}

void MovingObject::renderEditor(float lag, bool inPalette) {
    render(lag);
    if (!inPalette) {
        auto color = ColorUtils::setAlpha(ColorUtils::GRAY, 120);
        renderAt(lag, color, position + data.goal);
    }
}

std::shared_ptr<ObjectBase> MovingObject::clone() {
    return std::make_shared<MovingObject>(data);
}

#ifndef NDEBUG
void MovingObject::initTileEditorData(std::vector<TileEditorProp>& props) {
    props.insert(props.end(),
                 {TileEditorProp::Int("Size X", data.size.x, 0, 40),
                  TileEditorProp::Int("Size Y", data.size.y, 0, 40),
                  TileEditorProp::Float("Goal X", data.goal.x, -20.f, 20.f),
                  TileEditorProp::Float("Goal Y", data.goal.y, -20.f, 20.f),
                  TileEditorProp::Float("Speed", data.speed, 0.0f, 0.2f, 0.005f),
                  TileEditorProp::Bool("LEFT", data.spiky[static_cast<int>(Face::LEFT)]),
                  TileEditorProp::Bool("RIGHT", data.spiky[static_cast<int>(Face::RIGHT)]),
                  TileEditorProp::Bool("UP", data.spiky[static_cast<int>(Face::UP)]),
                  TileEditorProp::Bool("DOWN", data.spiky[static_cast<int>(Face::DOWN)])});
}

void MovingObject::applyTileEditorData(float* props) {
    data.size.x = props[0];
    data.size.y = props[1];
    data.goal.x = props[2];
    data.goal.y = props[3];
    data.speed = props[4];
    data.spiky[0] = props[5];
    data.spiky[1] = props[6];
    data.spiky[2] = props[7];
    data.spiky[3] = props[8];
}
#endif

Vector MovingObject::getSize() const {
    return data.size;
}

bool MovingObject::hasMoved() const {
    Vector diff = lastPosition - position;
    return diff.x != 0 || diff.y != 0;
}
