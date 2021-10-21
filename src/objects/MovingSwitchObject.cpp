#include "MovingSwitchObject.h"

#include "Objects.h"
#include "objects/ObjectRenderer.h"
#include "player/Player.h"

MovingSwitchObject::MovingSwitchObject() {
}

MovingSwitchObject::MovingSwitchObject(const Vector& size, const Vector& a, const Vector& b,
                                       float speed, bool seen)
    : MovingObject(size, a, b, speed), seen(seen) {
}

void MovingSwitchObject::postInit() {
    hiddenParticles =
        Objects::instantiateObject<ParticleSystem>("assets/particlesystems/switchobjectOFF.cmob");
    seenParticles =
        Objects::instantiateObject<ParticleSystem>("assets/particlesystems/switchobjectON.cmob");
}

void MovingSwitchObject::tick() {
    constexpr Color color[2] = {ColorUtils::BLACK, ColorUtils::WHITE};
    Vector size = this->getSize();
    if (isSolid()) {
        MovingObject::tick();
        seenParticles->play();
        hiddenParticles->stop();
        seenParticles->data.startColor = color[seen];
        seenParticles->data.endColor = color[seen];
        seenParticles->data.boxSize = size * 1.1f;
        seenParticles->data.minStartVelocity = -size * 0.01f;
        seenParticles->data.maxStartVelocity = size * 0.01f;
        seenParticles->position = position + size / 2.0f;
    } else {
        lastPosition = position;
        seenParticles->stop();
        hiddenParticles->play();
        hiddenParticles->data.startColor = color[!seen];
        hiddenParticles->data.endColor = color[!seen];
        hiddenParticles->data.boxSize = size;
        hiddenParticles->position = position + size / 2.0f;
        seenParticles->data.minStartVelocity = -size * 0.05f;
        seenParticles->data.maxStartVelocity = size * 0.05f;
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
    MovingObject::render(lag, color[seen]);
}

void MovingSwitchObject::renderEditor(float lag) {
    constexpr Color color[2] = {ColorUtils::DARK_GRAY, ColorUtils::LIGHT_GRAY};
    MovingObject::render(lag, color[seen]);
}

std::shared_ptr<ObjectBase> MovingSwitchObject::clone() {
    return std::make_shared<MovingSwitchObject>(data.size, data.goalA, data.goalB, data.speed,
                                                seen);
}

void MovingSwitchObject::read(std::ifstream& in) {
    MovingObject::read(in);
    in.read(reinterpret_cast<char*>(&seen), sizeof(seen));
    seen &= 1;
}

void MovingSwitchObject::write(std::ofstream& out) {
    MovingObject::write(out);
    out.write(reinterpret_cast<char*>(&seen), sizeof(seen));
}
