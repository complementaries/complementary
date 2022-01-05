#include "GoalTile.h"

#include "Game.h"
#include "GoalCutscene.h"
#include "player/Player.h"
#include "tilemap/Tilemap.h"
#include "tilemap/Tiles.h"

std::shared_ptr<ParticleSystem> GoalTile::prototype;

void GoalTile::init() {
    prototype = Objects::loadObject<ParticleSystem>("assets/particlesystems/goal.cmob");
}

GoalTile::GoalTile(Face face) : Tile(ColorUtils::BLACK, false, "default"), face(face) {
}

void GoalTile::onCollision(int x, int y) const {
    if (!GoalCutscene::isActive()) {
        GoalCutscene::show(Vector(x, y), face);
        if (Game::getCurrentLevel() == -1) {
            Game::setLevelScreenPosition(Player::getPosition() + FaceUtils::getDirection(face) * 2);
        }
    }
}

bool GoalTile::isWall() const {
    return false;
}

void GoalTile::render(Buffer& buffer, float x, float y, float z) const {
    if (z == -0.2f) {
        z = -0.1f;
    }
    const Color color[] = {getColor(), ColorUtils::setAlpha(ColorUtils::invert(getColor()), 0)};
    buffer.add(x).add(y).add(z).add(color[face == Face::LEFT || face == Face::UP]);
    buffer.add(x).add(y + 1.0f).add(z).add(color[face == Face::LEFT || face == Face::DOWN]);
    buffer.add(x + 1.0f).add(y).add(z).add(color[face == Face::RIGHT || face == Face::UP]);
    buffer.add(x + 1.0f).add(y + 1.0f).add(z).add(color[face == Face::RIGHT || face == Face::DOWN]);
    buffer.add(x).add(y + 1.0f).add(z).add(color[face == Face::LEFT || face == Face::DOWN]);
    buffer.add(x + 1.0f).add(y).add(z).add(color[face == Face::RIGHT || face == Face::UP]);
}

void GoalTile::onLoad(int x, int y) const {
    std::shared_ptr<ParticleSystem> p = Objects::instantiateClone(GoalTile::prototype);
    constexpr float v = 0.05f;
    switch (face) {
        case Face::UP:
            p->position = Vector(x + 0.5f, y - 2.0f);
            p->data.boxSize = Vector(1.0f - p->data.startSize, 0.0f);
            p->data.minStartVelocity = Vector(0.0f, v);
            p->data.maxStartVelocity = Vector(0.0f, v * 0.5f);
            break;
        case Face::DOWN:
            p->position = Vector(x + 0.5f, y + 3.0f);
            p->data.boxSize = Vector(1.0f - p->data.startSize, 0.0f);
            p->data.minStartVelocity = Vector(0.0f, -v * 0.5f);
            p->data.maxStartVelocity = Vector(0.0f, -v);
            break;
        case Face::LEFT:
            p->position = Vector(x - 2.0f, y + 0.5f);
            p->data.boxSize = Vector(0.0f, 1.0f - p->data.startSize);
            p->data.minStartVelocity = Vector(v, 0.0f);
            p->data.maxStartVelocity = Vector(v * 0.5f, 0.0f);
            break;
        default:
        case Face::RIGHT:
            p->position = Vector(x + 3.0f, y + 0.5f);
            p->data.boxSize = Vector(0.0f, 1.0f - p->data.startSize);
            p->data.minStartVelocity = Vector(-v * 0.5f, 0.0f);
            p->data.maxStartVelocity = Vector(-v, 0.0f);
            break;
    }
    p->play();
}
