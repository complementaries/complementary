#include "WindObject.h"

#include "ObjectRenderer.h"
#include "Objects.h"
#include "player/Player.h"
#include "sound/SoundManager.h"
#include <iostream>
#include <memory>

static std::vector<float> windObjects;

WindObject::WindObject() {
}
WindObject::~WindObject() {
    if (this->index) {
        windObjects.clear();
    }
}

WindObject::WindObject(const WindObjectData& data) {
    this->data = data;
    if (windObjects.empty()) {
        this->index = 0;
    } else {
        this->index = windObjects.size();
    }
    std::cout << this->index << std::endl;
    windObjects.push_back(biggestFloat);
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

void WindObject::tick() {
    handleSound(Sound::WIND);
    constexpr Color colors[] = {ColorUtils::DARK_GRAY, ColorUtils::LIGHT_GRAY};
    particles->data.startColor = ColorUtils::setAlpha(colors[Player::invertColors()], 150);
    particles->data.endColor = ColorUtils::setAlpha(colors[Player::invertColors()], 0);
}

void WindObject::postInit() {
    particles = Objects::instantiateObject<ParticleSystem>("assets/particlesystems/wind.cmob");
    particles->data.minStartVelocity = data.force * 2.0f;
    particles->data.maxStartVelocity = data.force * 2.0f;
    particles->data.boxSize = data.size;
    particles->data.maxLifetime = std::max(data.size.x, data.size.y) * 10;
    particles->position = this->position + data.size / 2;
    particles->data.boxLifetimeLoss = 4;
    particles->play();
}

void WindObject::handleSound(int soundId) {
    float distance = calculatePlayerDistance();
    float xDistance = calculatePlayerDistanceAxis(0);
    if (abs(distance) < soundThreshold) {
        windObjects[this->index] = distance;
        if (!SoundManager::soundPlaying(soundId)) {
            SoundManager::playContinuousSound(soundId);
        }
        if (nearestWind() == abs(distance)) {
            SoundManager::setDistanceToPlayer(soundId, distance, xDistance, soundThreshold);
        }
    } else {
        windObjects[this->index] = biggestFloat;
        if (SoundManager::soundPlaying(soundId) && nearestWind() == biggestFloat) {
            SoundManager::stopSound(soundId);
        }
    }
}

float WindObject::nearestWind() {
    float smallestDistance = biggestFloat;
    for (int i = 0; i < windObjects.size(); i++) {
        float currDistance = abs(windObjects[i]);
        if (smallestDistance > currDistance) {
            smallestDistance = currDistance;
        }
    }
    return smallestDistance;
}

float WindObject::calculatePlayerDistance() {
    float xDist = calculatePlayerDistanceAxis(0);
    float yDist = calculatePlayerDistanceAxis(1);

    // float distance =
    //    pow(pow(playerPos.x - position.x, 2.0f) + pow(playerPos.y - position.y, 2.0f), 0.5f);
    float distance = abs(xDist) > abs(yDist) ? xDist : yDist;
    return distance;
}

float WindObject::calculatePlayerDistanceAxis(int axis) {
    Vector playerPos = Player::getPosition();
    float distance = 0;
    if (playerPos[axis] < position[axis]) {
        distance = playerPos[axis] - position[axis];
    } else if (playerPos[axis] > position[axis] + data.size[axis]) {
        distance = playerPos[axis] - (position[axis] + data.size[axis]);
    }
    return distance;
}

bool WindObject::collidesWith(const Vector& pPosition, const Vector& pSize) const {
    return position[0] < pPosition[0] + pSize[0] && position[0] + data.size[0] > pPosition[0] &&
           position[1] < pPosition[1] + pSize[1] && position[1] + data.size[1] > pPosition[1];
}

void WindObject::renderEditor(float lag, bool inPalette) {
    (void)lag;
    (void)inPalette;
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
    props.insert(props.end(),
                 {
                     TileEditorProp::Int("Size X", data.size.x, 0, 80),
                     TileEditorProp::Int("Size Y", data.size.y, 0, 40),
                     TileEditorProp::Float("Force X", data.force.x, -0.5f, 0.5f, 0.01f),
                     TileEditorProp::Float("Force Y", data.force.y, -0.5f, 0.5f, 0.01f),
                 });
}

void WindObject::applyTileEditorData(float* props) {
    data.size.x = props[0];
    data.size.y = props[1];
    data.force.x = props[2];
    data.force.y = props[3];
}
#endif

Vector WindObject::getSize() const {
    return data.size;
}
