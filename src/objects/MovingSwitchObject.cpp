#include "MovingSwitchObject.h"

#include "objects/ObjectRenderer.h"
#include "player/Player.h"

MovingSwitchObject::MovingSwitchObject() {
}

MovingSwitchObject::MovingSwitchObject(const Vector& size, const Vector& a, const Vector& b,
                                       float speed, bool seen)
    : MovingObject(size, a, b, speed), seen(seen) {
}

void MovingSwitchObject::tick() {
    if (isSolid()) {
        MovingObject::tick();
    } else {
        lastPosition = position;
    }
}

bool MovingSwitchObject::isSolid() const {
    return seen == Player::invertColors();
}

void MovingSwitchObject::render(float lag) {
    if (!isSolid()) {
        return;
    }
    constexpr Color color[2] = {ColorUtils::BLACK, ColorUtils::WHITE};
    Vector oversize(0.05f, 0.05f);
    Vector i = lastPosition + (position - lastPosition) * lag;
    ObjectRenderer::drawRectangle(i - oversize, data.size + oversize * 2.0f, color[seen]);
}

void MovingSwitchObject::renderEditor(float lag) {
    constexpr Color color[2] = {ColorUtils::rgba(20, 20, 20), ColorUtils::rgba(235, 235, 235)};
    Vector oversize(0.05f, 0.05f);
    Vector i = position;
    ObjectRenderer::drawRectangle(i - oversize, data.size + oversize * 2.0f, color[seen]);
}

std::shared_ptr<ObjectBase> MovingSwitchObject::clone() {
    return std::make_shared<MovingSwitchObject>(data.size, data.goalA, data.goalB, data.speed,
                                                seen);
}

void MovingSwitchObject::read(std::ifstream& in) {
    MovingObject::read(in);
    in.read(reinterpret_cast<char*>(&seen), sizeof(seen));
}

void MovingSwitchObject::write(std::ofstream& out) {
    MovingObject::write(out);
    out.write(reinterpret_cast<char*>(&seen), sizeof(seen));
}