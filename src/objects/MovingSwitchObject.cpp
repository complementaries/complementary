#include <array>

#include "MovingSwitchObject.h"

#include "Objects.h"
#include "objects/ObjectRenderer.h"
#include "player/Player.h"

MovingSwitchObject::MovingSwitchObject() {
}

MovingSwitchObject::MovingSwitchObject(const Vector& size, const Vector& goal, float speed,
                                       bool seen)
    : MovingObject(size, goal, speed), seen(seen) {
}

void MovingSwitchObject::postInit() {
    MovingObject::postInit();
    hiddenParticles =
        Objects::instantiateObject<ParticleSystem>("assets/particlesystems/switchobjectOFF.cmob");
    seenParticles =
        Objects::instantiateObject<ParticleSystem>("assets/particlesystems/switchobjectON.cmob");
    Vector size = this->getSize();
    seenParticles->data.boxSize = size * 1.1f;
    seenParticles->data.minStartVelocity = Vector(-1.0f, -1.0f) * 0.035f;
    seenParticles->data.maxStartVelocity = Vector(1.0f, 1.0f) * 0.035f;
    seenParticles->data.minEmissionRate = std::min(50.f, 2.0f * size.x * size.y);

    hiddenParticles->data.boxSize = size;
    hiddenParticles->data.minEmissionRate = std::min(50.f, 5.0f * size.x * size.y);
}

void MovingSwitchObject::tick() {
    std::array<bool, 4> spikes;
    spikes[0] = data.spiky[0];
    spikes[1] = data.spiky[1];
    spikes[2] = data.spiky[2];
    spikes[3] = data.spiky[3];
    hiddenParticles->setSpikes(spikes);
    seenParticles->setSpikes(spikes);

    constexpr Color color[2] = {ColorUtils::BLACK, ColorUtils::WHITE};
    Vector size = this->getSize();
    if (isSolid()) {
        MovingObject::tick();
        if (!seenParticles->isPlaying()) {
            seenParticles->play();
        }
        hiddenParticles->stop();
        seenParticles->data.startColor = color[seen];
        seenParticles->data.endColor = ColorUtils::setAlpha(color[seen], 150);

        seenParticles->position = position + size / 2.0f;
    } else {
        lastPosition = position;
        seenParticles->stop();
        if (!hiddenParticles->isPlaying()) {
            hiddenParticles->play();
        }
        hiddenParticles->data.startColor = ColorUtils::setAlpha(color[!seen], 0);
        hiddenParticles->data.endColor = color[!seen];
        hiddenParticles->position = position + size / 2.0f;
    }
}

bool MovingSwitchObject::isSolid() const {
    return seen == Player::invertColors();
}

bool MovingSwitchObject::isSolidInAnyWorld() const {
    return true;
}

void MovingSwitchObject::render(float lag) {
    if (!isSolid()) {
        return;
    }
    constexpr Color color[2] = {ColorUtils::BLACK, ColorUtils::WHITE};
    MovingObject::render(lag, color[seen]);
}

void MovingSwitchObject::renderEditor(float lag, bool inPalette) {
    constexpr Color colors[2] = {ColorUtils::DARK_GRAY, ColorUtils::LIGHT_GRAY};
    Color color = colors[seen];
    MovingObject::render(lag, color);

    if (!inPalette) {
        color = ColorUtils::setAlpha(color, 120);
        renderAt(lag, color, position + data.goal);
    }
}

std::shared_ptr<ObjectBase> MovingSwitchObject::clone() {
    return std::make_shared<MovingSwitchObject>(data.size, data.goal, data.speed, seen);
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
