#include "WindObject.h"

#include <unordered_map>

#include "ObjectRenderer.h"
#include "Objects.h"
#include "player/Player.h"
#include "sound/SoundManager.h"

std::unordered_map<int, float> windObjects;
static int nextIndex = 0;

WindObject::WindObject() : index(nextIndex++) {
    windObjects[index] = biggestFloat;
}

WindObject::~WindObject() {
    windObjects.erase(index);
    if (windObjects.empty() && SoundManager::soundPlaying(Sound::WIND)) {
        SoundManager::stopSound(Sound::WIND);
    }
}

WindObject::WindObject(const WindObjectData& data) : WindObject() {
    this->data = data;
}

WindObject::WindObject(const Vector& position, const Vector& size, const Vector& force)
    : WindObject() {
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
    particles->data.startColor = ColorUtils::setAlpha(colors[Player::invertColors()], 80);
    particles->data.endColor = ColorUtils::setAlpha(colors[Player::invertColors()], 0);
}

void WindObject::postInit() {
    particles = Objects::instantiateObject<ParticleSystem>("assets/particlesystems/wind.cmob");
    particles->data.minStartVelocity = data.force * 1.0f;
    particles->data.maxStartVelocity = data.force * 2.0f;
    particles->data.boxSize = Vector(abs(data.force.y) < 0.001f ? 0 : data.size.x,
                                     abs(data.force.x) < 0.001f ? 0 : data.size.y);
    particles->data.clampBoxSize = data.size;
    particles->data.maxLifetime = (data.force.x > data.force.y ? data.size.x : data.size.y) * 100;
    particles->position = this->position + data.size / 2;
    particles->data.boxLifetimeLoss = 20;
    particles->data.maxEmissionInterval =
        std::max(1, static_cast<int>(30 - (abs(data.force.x) + abs(data.force.y)) * 300));
    particles->data.maxEmissionRate =
        6 * std::max(1, static_cast<int>((particles->data.boxSize.x + particles->data.boxSize.y) /
                                         10.0f));
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
    for (auto& windObject : windObjects) {
        float currDistance = abs(windObject.second);
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
    ObjectRenderer::addRectangle(position, data.size, ColorUtils::rgba(255, 0, 0, 100));

    // TEMP: indicator for the wind direction
    auto normForce = data.force;
    normForce.normalize();
    ObjectRenderer::addRectangle(position + data.size * 0.5f, Vector(1.f, 1.f),
                                 ColorUtils::rgba(255, 0, 0, 200));
    ObjectRenderer::addRectangle(position + data.size * 0.5f + normForce + Vector(0.2f, 0.2f),
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
