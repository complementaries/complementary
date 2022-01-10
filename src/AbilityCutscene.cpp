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
#include "sound/SoundManager.h"
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

void AbilityCutscene::show(Ability previous) {
    currentTicks = 0;
    alpha = 0;
    Player::setGravityEnabled(false);
    Player::setOverrideColor(AbilityUtils::getColor(previous));
    Input::playRumble(0.75f, 2500);
    SoundManager::playSoundEffect(Sound::NEW_ABILITY);
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
        Color playerColor = ColorUtils::mix(Player::getOverrideColor(),
                                            AbilityUtils::getColor(Player::getAbility()),
                                            (static_cast<float>(currentTicks) - 50.f) / 150.f);

        Player::setOverrideColor(playerColor);
    }
    if (currentTicks >= 50 && currentTicks <= 220) {
        RenderState::addRandomizedShake(2.0f);
    }

    if (currentTicks < 100) {
        RenderState::setZoom(1.0f + (static_cast<float>(currentTicks) / 100));
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
        Game::setFade(static_cast<float>(alpha) * 0.55f);
    }

    if (currentTicks > 550 && alpha > 0) {
        alpha -= 5;
        Game::setFade(static_cast<float>(alpha) * 0.55f);
    }

    if (currentTicks > 600) {
        RenderState::setZoom(2.0f - (static_cast<float>(currentTicks - 600) / 100));
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
    float smooth = std::min(0.15f * 1500.0f / Window::getWidth(), 0.95f);
    TextureRenderer::renderIcon(iconPos, iconPos + size, ability, alpha, smooth);

    Font::prepare();

    Color col = AbilityUtils::getColor(ability);
    col = ColorUtils::setAlpha(col, alpha);
    float row1Width = Font::getWidth(2, AbilityUtils::getName(ability));
    Vector row1Pos(wSize.x / 2 - row1Width / 2, wSize.y / 2 + 3.f);
    Font::draw(row1Pos, 2, col, AbilityUtils::getName(ability));

    char description[128] = {};
    AbilityUtils::getDescription(ability, description, 128);

    float row2Width = Font::getWidth(1, description);
    Vector row2Pos(wSize.x / 2 - row2Width / 2, wSize.y / 2 + 5.5f);
    Font::draw(row2Pos, 1, col, description);
}

bool AbilityCutscene::isActive() {
    return currentTicks >= 0 && currentTicks <= 700;
}
