#include "AbilityCutscene.h"

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

bool AbilityCutscene::init() {
    particles =
        Objects::instantiateObject<ParticleSystem>("assets/particlesystems/newability.cmob");
    if (particles == nullptr) {
        return true;
    }
    particles->destroyOnLevelLoad = false;
    return false;
}

void AbilityCutscene::show() {
    currentTicks = 0;
    Player::setGravityEnabled(false);
    Player::setOverrideColor(ColorUtils::GRAY);
}

void AbilityCutscene::tick() {
    if (!isActive()) {
        return;
    }

    if (currentTicks < 50) {
        Player::addBaseVelocity(Vector(0, -0.04));
    }
    if (currentTicks == 25) {
        particles->play();
    }
    if (currentTicks >= 50 && currentTicks < 200) {
        Player::addBaseVelocity(Vector(0, sinf((currentTicks - 50) * 0.02f) * 0.01));
        Color playerColor =
            ColorUtils::mix(ColorUtils::GRAY, AbilityUtils::getColor(Player::getAbility()),
                            (static_cast<float>(currentTicks) - 50.f) / 150.f);

        Player::setOverrideColor(playerColor);
    }

    if (currentTicks == 200) {
        Player::resetOverrideColor();
        particles->stop();
    }

    if (currentTicks == 220) {
        Player::setGravityEnabled(true);
    }

    if (currentTicks == 400 || currentTicks == 600) {
        Game::switchWorld();
    }

    if (currentTicks > 240 && currentTicks < 300 && alpha < 255) {
        alpha += 5;
    }

    if (currentTicks > 550 && alpha > 0) {
        alpha -= 5;
    }

    PlayerParticles::setParticleColor(particles);
    currentTicks++;
}

void AbilityCutscene::render(float lag) {
    (void)lag;

    if (!isActive()) {
        return;
    }

    Vector wSize(Tilemap::getWidth(), Tilemap::getHeight());
    Vector size(8.0f, 8.0f);
    Ability ability = Player::getAbility();
    Vector iconPos(wSize.x / 2 - size.x / 2, wSize.y / 2 - size.x / 2 - 2.f);
    TextureRenderer::renderIcon(iconPos, iconPos + size, ability, alpha);

    Font::prepare();
    float width = Font::getWidth(2, AbilityUtils::getName(ability));
    Vector textPos(wSize.x / 2 - width / 2, wSize.y / 2 + 3.f);
    Color col = AbilityUtils::getColor(ability);
    col = ColorUtils::setAlpha(col, alpha);
    Font::draw(textPos, 2, col, AbilityUtils::getName(ability));
}

bool AbilityCutscene::isActive() {
    return currentTicks >= 0 && currentTicks <= 600;
}