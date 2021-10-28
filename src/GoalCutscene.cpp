#include "GoalCutscene.h"

#include <cmath>
#include <string>
#include <vector>

#include "Game.h"
#include "Input.h"
#include "graphics/Font.h"
#include "graphics/RenderState.h"
#include "graphics/TextureRenderer.h"
#include "graphics/Window.h"
#include "objects/Objects.h"
#include "particles/ParticleSystem.h"
#include "player/Player.h"
#include "tilemap/Tilemap.h"

static int currentTicks = -1;
static int alpha = 0;
static std::shared_ptr<ParticleSystem> particles;
static Vector goalPosition;
static float initialAttactSpeed;
static Color initialStartColor;
static Color initialEndColor;
static Vector playerPosBeforeDissolve;
static bool dashing;

bool GoalCutscene::init() {
    particles = Objects::instantiateObject<ParticleSystem>("assets/particlesystems/dissolve.cmob");
    if (particles == nullptr) {
        return true;
    }
    particles->destroyOnLevelLoad = false;
    initialAttactSpeed = particles->data.attractSpeed;
    initialStartColor = particles->data.startColor;
    initialEndColor = particles->data.endColor;
    return false;
}

void GoalCutscene::show(Vector inGoalPosition, Face goalFace) {
    currentTicks = 0;
    alpha = 0;
    goalPosition = inGoalPosition;
    switch (goalFace) {
        case Face::UP: goalPosition.y += 2; break;
        case Face::DOWN: goalPosition.y -= 2; break;
        case Face::LEFT: goalPosition.x += 2; break;
        case Face::RIGHT: goalPosition.x -= 2; break;
        default: break;
    }
    goalPosition += Vector(0.5f, 0.5f);

    particles->data.attractSpeed = initialAttactSpeed;
    particles->data.startColor = initialStartColor;
    particles->data.endColor = initialEndColor;
    particles->data.enableCollision = true;

    dashing = Player::isDashing();
    Player::setAbilities(Ability::NONE, Ability::NONE, true);
    Player::resetVelocity();
    Player::setGravityEnabled(false);
}

void GoalCutscene::tick() {
    if (!isActive()) {
        return;
    }

    Player::resetVelocity();

    if (currentTicks > 20 && currentTicks < 70) {
        RenderState::setZoom(1.0f + (static_cast<float>(currentTicks - 20) / 100));
    }

    if ((currentTicks == 90 && !dashing) || (currentTicks == 0 && dashing)) {
        Player::setHidden(true);
        PlayerParticles::setParticlePosition(particles, 0, 0, 0, 0);
        RenderState::addRandomizedShake(4.f);
        particles->play();
        playerPosBeforeDissolve = Player::getPosition();
    }

    if (dashing && currentTicks < 90) {
        Color targetCol = Player::invertColors() ? ColorUtils::WHITE : ColorUtils::BLACK;
        particles->data.startColor =
            ColorUtils::mix(initialStartColor, targetCol, static_cast<float>(currentTicks) / 90.f);
        particles->data.endColor = particles->data.startColor;
    }

    if (currentTicks == 90 && dashing) {
        // Skip part of the animation if the player dashed into the goal
        currentTicks += 110;
        particles->position = goalPosition;
        particles->data.attractSpeed = 0.002f;
        particles->data.enableCollision = false;
        playerPosBeforeDissolve = Player::getPosition();
    }

    if (currentTicks >= 100 && currentTicks < 200 && !dashing) {
        Color targetCol = Player::invertColors() ? ColorUtils::WHITE : ColorUtils::BLACK;
        particles->data.startColor = ColorUtils::mix(
            initialStartColor, targetCol, static_cast<float>(currentTicks - 100) / 100.f);
        particles->data.endColor = particles->data.startColor;
    }

    if (currentTicks == 200 && !dashing) {
        particles->position = goalPosition;
        particles->data.attractSpeed = 0.002f;
        particles->data.enableCollision = false;
    }

    if (currentTicks > 200) {
        alpha += 3;
        alpha = std::min(255, alpha);
        Game::setFade(alpha);

        Player::setPosition(Vector::lerp(playerPosBeforeDissolve, goalPosition,
                                         static_cast<float>(currentTicks - 200) / 300.f));
        RenderState::addRandomizedShake(0.4f);
    }

    if (currentTicks >= 300) {
        Player::setGravityEnabled(true);
        Player::setHidden(false);
        Player::resetDash();
        Game::nextLevel();
        RenderState::setZoom(1.0f);
    }

    currentTicks++;
}

void GoalCutscene::render(float lag) {
    (void)lag;
}

bool GoalCutscene::isActive() {
    return currentTicks >= 0 && currentTicks <= 300;
}
