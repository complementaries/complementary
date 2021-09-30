#include "MovingObject.h"

#include "ObjectRenderer.h"
#include "objects/Objects.h"
#include "player/Player.h"

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
    data.spiky[0] = true;
    data.spiky[1] = false;
    data.spiky[2] = false;
    data.spiky[3] = false;
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

    Vector step = velocity;
    if (step.y != 0.0f) {
        step.x = 0.0f;
    }
    step.normalize();
    step *= 0.01f;
    while (Player::isColliding(*this)) {
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
    if (data.spiky[FaceUtils::invert(playerFace)]) {
        Player::kill();
    }
}

bool MovingObject::collidesWith(const Vector& pPosition, const Vector& pSize) const {
    return position[0] < pPosition[0] + pSize[0] && position[0] + data.size[0] > pPosition[0] &&
           position[1] < pPosition[1] + pSize[1] && position[1] + data.size[1] > pPosition[1];
}

void MovingObject::render(float lag, Color color) {
    Vector offset(0.33f, 0.33f);
    Vector size = data.size - offset * 2.0f;
    Vector p = lastPosition + (position - lastPosition) * lag;
    ObjectRenderer::drawRectangle(p + offset, size, color);
    if (data.spiky[Face::UP]) {
        for (int i = data.size.x * 3 - 1; i > 1; i--) {
            Vector a = p + Vector(0.3333f * i, 0.33f);
            Vector b = p + Vector(0.3333f * (i - 1), 0.33f);
            Vector c = p + Vector(0.3333f * (i - 1) + 0.165f, 0.0f);
            ObjectRenderer::drawTriangle(a, b, c, color);
        }
    } else {
        Vector a = p - Vector(0.0f, 0.0f);
        Vector b = p + Vector(0.5f, 0.4f);
        Vector c = p + Vector(data.size.x, 0.0f);
        ObjectRenderer::drawTriangle(a, b, c, color);
        a = p + Vector(data.size.x - 0.5f, 0.4f);
        ObjectRenderer::drawTriangle(a, b, c, color);
        ObjectRenderer::drawRectangle(p - Vector(0.0f, 0.05f), Vector(data.size.x, 0.05f), color);
    }
    if (data.spiky[Face::DOWN]) {
        for (int i = data.size.x * 3 - 1; i > 1; i--) {
            Vector a = p + Vector(0.3333f * i, data.size.y - 0.33f);
            Vector b = p + Vector(0.3333f * (i - 1), data.size.y - 0.33f);
            Vector c = p + Vector(0.3333f * (i - 1) + 0.165f, data.size.y);
            ObjectRenderer::drawTriangle(a, b, c, color);
        }
    } else {
        Vector a = p + Vector(0.0f, data.size.y);
        Vector b = p + Vector(0.5f, data.size.y - 0.4f);
        Vector c = p + data.size;
        ObjectRenderer::drawTriangle(a, b, c, color);
        a = p + data.size - Vector(0.5f, 0.4f);
        ObjectRenderer::drawTriangle(a, b, c, color);
    }
    if (data.spiky[Face::LEFT]) {
        for (int i = data.size.y * 3 - 1; i > 1; i--) {
            Vector a = p + Vector(0.3333f, 0.3333f * i);
            Vector b = p + Vector(0.3333f, 0.3333f * (i - 1));
            Vector c = p + Vector(0.0f, 0.3333f * (i - 1) + 0.165f);
            ObjectRenderer::drawTriangle(a, b, c, color);
        }
    } else {
        Vector a = p;
        Vector b = p + Vector(0.4f, 0.5f);
        Vector c = p + Vector(0.0f, data.size.y);
        ObjectRenderer::drawTriangle(a, b, c, color);
        a = p + Vector(0.4f, data.size.y - 0.5f);
        ObjectRenderer::drawTriangle(a, b, c, color);
    }
    if (data.spiky[Face::RIGHT]) {
        for (int i = data.size.y * 3 - 1; i > 1; i--) {
            Vector a = p + Vector(data.size.x - 0.3333f, 0.3333f * i);
            Vector b = p + Vector(data.size.x - 0.3333f, 0.3333f * (i - 1));
            Vector c = p + Vector(data.size.x, 0.3333f * (i - 1) + 0.165f);
            ObjectRenderer::drawTriangle(a, b, c, color);
        }
    } else {
        Vector a = p + Vector(data.size.x, 0.0f);
        Vector b = p + Vector(data.size.x - 0.4f, 0.5f);
        Vector c = p + Vector(data.size.x, data.size.y);
        ObjectRenderer::drawTriangle(a, b, c, color);
        a = p + Vector(data.size.x - 0.4f, data.size.y - 0.5f);
        ObjectRenderer::drawTriangle(a, b, c, color);
    }

    Vector a = p;
    Vector b = p + Vector(0.5f, 0.3f);
    Vector c = p + Vector(0.3f, 0.5f);
    ObjectRenderer::drawTriangle(a, b, c, color);

    a = p + Vector(data.size.x, 0.0f);
    b = p + Vector(data.size.x - 0.5f, 0.3f);
    c = p + Vector(data.size.x - 0.3f, 0.5f);
    ObjectRenderer::drawTriangle(a, b, c, color);

    a = p + Vector(0.0f, data.size.y);
    b = p + Vector(0.5f, data.size.y - 0.3f);
    c = p + Vector(0.3f, data.size.y - 0.5f);
    ObjectRenderer::drawTriangle(a, b, c, color);

    a = p + data.size;
    b = p + Vector(data.size.x - 0.5f, data.size.y - 0.3f);
    c = p + Vector(data.size.x - 0.3f, data.size.y - 0.5f);
    ObjectRenderer::drawTriangle(a, b, c, color);
}

void MovingObject::render(float lag) {
    render(lag, ColorUtils::GRAY);
}

void MovingObject::renderEditor(float lag) {
    render(lag);
}

std::shared_ptr<ObjectBase> MovingObject::clone() {
    return std::make_shared<MovingObject>(data);
}

#ifndef NDEBUG
void MovingObject::initTileEditorData(std::vector<TileEditorProp>& props) {
    props.insert(props.end(), {TileEditorProp::Int("Size X", data.size.x, 0, 5),
                               TileEditorProp::Int("Size Y", data.size.y, 0, 5),
                               TileEditorProp::Float("Goal 1 X", data.goalA.x, 0.f, 200.f),
                               TileEditorProp::Float("Goal 1 Y", data.goalA.y, 0.f, 200.f),
                               TileEditorProp::Float("Goal 2 X", data.goalB.x, 0.f, 200.f),
                               TileEditorProp::Float("Goal 2 Y", data.goalB.y, 0.f, 200.f),
                               TileEditorProp::Float("Speed", data.speed, 0.f, 0.5f),
                               TileEditorProp::Bool("LEFT", data.spiky[Face::LEFT]),
                               TileEditorProp::Bool("RIGHT", data.spiky[Face::RIGHT]),
                               TileEditorProp::Bool("UP", data.spiky[Face::UP]),
                               TileEditorProp::Bool("DOWN", data.spiky[Face::DOWN])});
}

void MovingObject::applyTileEditorData(float* props) {
    data.size.x = props[0];
    data.size.y = props[1];
    data.goalA.x = props[2];
    data.goalA.y = props[3];
    data.goalB.x = props[4];
    data.goalB.y = props[5];
    data.speed = props[6];
    data.spiky[0] = props[7];
    data.spiky[1] = props[8];
    data.spiky[2] = props[9];
    data.spiky[3] = props[10];
}
#endif

Vector MovingObject::getSize() const {
    return data.size;
}