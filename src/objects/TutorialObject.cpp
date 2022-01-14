#include "TutorialObject.h"

#include "Game.h"
#include "Input.h"
#include "Menu.h"
#include "ObjectRenderer.h"
#include "Savegame.h"
#include "TextUtils.h"
#include "graphics/Font.h"
#include "graphics/gl/Glew.h"
#include "objects/Objects.h"
#include "player/Player.h"
#include "sound/SoundManager.h"

#include <cmath>
#include <memory>

static constexpr int DISPLAY_DELAY = 200;

TutorialObject::TutorialObject(Vector position, Vector size)
    : alpha(0.f), ticksUntilDisplayed(0), completed(false), wasCollidingInLastFrame(false) {
    this->position = position;
    data = {};
    data.size = size;
}

TutorialObject::TutorialObject(TutorialObjectData data)
    : alpha(0.f), ticksUntilDisplayed(0), completed(false), wasCollidingInLastFrame(false) {
}

void TutorialObject::tick() {
    if (Game::isInSpeedrun()) {
        return;
    }
    bool colliding = Player::isColliding(*this);
    if (colliding && !wasCollidingInLastFrame) {
        ticksUntilDisplayed = DISPLAY_DELAY;
    }
    if (Player::isAllowedToMove() && colliding && shouldShowTutorial() && !completed &&
        (ticksUntilDisplayed <= 0 || data.instant)) {
        completed = checkTutorialCompleted();
        alpha += 10.f;
    } else {
        alpha -= 0.5f;
    }
    alpha = std::max(std::min(alpha, 255), 0);

    ticksUntilDisplayed--;
    wasCollidingInLastFrame = colliding;
}

void TutorialObject::onCollision() {
}

bool TutorialObject::collidesWith(const Vector& pPosition, const Vector& pSize) const {
    return position[0] < pPosition[0] + pSize[0] && position[0] + data.size[0] > pPosition[0] &&
           position[1] < pPosition[1] + pSize[1] && position[1] + data.size[1] > pPosition[1];
}

void TutorialObject::renderEditor(float lag, bool inPalette) {
    (void)lag;
    (void)inPalette;
    ObjectRenderer::addRectangle(position, data.size, ColorUtils::rgba(0, 0, 100, 100));
}

void TutorialObject::renderText(float lag) {
    if (alpha > 0) {
        char tutorialText[128];
        getTutorialText(tutorialText, 128);
        if (alpha < 150) {
            glDepthMask(false);
            TextUtils::drawPopupObjectSpace(Player::getPosition() +
                                                Vector(Player::getSize().x * 0.5f, 0.f),
                                            tutorialText, alpha);
            glDepthMask(true);
        } else {
            TextUtils::drawPopupObjectSpace(Player::getPosition() +
                                                Vector(Player::getSize().x * 0.5f, 0.f),
                                            tutorialText, alpha);
        }
    }
}

std::shared_ptr<ObjectBase> TutorialObject::clone() {
    return std::make_shared<TutorialObject>(data);
}

#ifndef NDEBUG
void TutorialObject::initTileEditorData(std::vector<TileEditorProp>& props) {
    props.insert(props.end(), {TileEditorProp::Int("Size X", data.size.x, 1, 30),
                               TileEditorProp::Int("Size Y", data.size.y, 1, 30),
                               TileEditorProp::Int("Type", static_cast<int>(data.type), 0,
                                                   static_cast<int>(TutorialType::MAX) - 1),
                               TileEditorProp::Bool("Instant", data.instant)});
}

void TutorialObject::applyTileEditorData(float* props) {
    data.size.x = props[0];
    data.size.y = props[1];
    data.type = static_cast<TutorialType>(props[2]);
    data.instant = props[3] != 0.f;
}
#endif

Vector TutorialObject::getSize() const {
    return data.size;
}

bool TutorialObject::shouldShowTutorial() const {
    switch (data.type) {
        case TutorialType::WORLD_SWITCH: return !Player::invertColors();
        case TutorialType::JUMP: return true;
        case TutorialType::DOUBLE_JUMP: return Player::hasAbility(Ability::DOUBLE_JUMP);
        case TutorialType::GLIDER: return Player::hasAbility(Ability::GLIDER);
        case TutorialType::DASH: return Player::hasAbility(Ability::DASH);
        case TutorialType::WALL_JUMP: return Player::hasAbility(Ability::WALL_JUMP);
        case TutorialType::DASH_SWITCH_COMBO:
            return Player::hasAbility(Ability::DASH) || Player::hasAbility(Ability::WALL_JUMP) ||
                   Player::hasAbility(Ability::GLIDER) || Player::hasAbility(Ability::DOUBLE_JUMP);
        default: return false;
    }
    return false;
}

bool TutorialObject::checkTutorialCompleted() const {
    switch (data.type) {
        case TutorialType::WORLD_SWITCH:
            return Input::getButton(ButtonType::SWITCH).pressed ||
                   Input::getButton(ButtonType::SWITCH_AND_ABILITY).pressed;
        case TutorialType::JUMP: return Input::getButton(ButtonType::JUMP).pressed;
        case TutorialType::DOUBLE_JUMP:
            return Input::getButton(ButtonType::JUMP).pressedFirstFrame && !Player::isGrounded();
        case TutorialType::WALL_JUMP:
            return Input::getButton(ButtonType::JUMP).pressedFirstFrame && Player::isWallSticking();
        case TutorialType::GLIDER:
            return (Input::getButton(ButtonType::ABILITY).pressed ||
                    Input::getButton(ButtonType::SWITCH_AND_ABILITY).pressed) &&
                   !Player::isGrounded();
        case TutorialType::DASH:
            return Input::getButton(ButtonType::ABILITY).pressed ||
                   Input::getButton(ButtonType::SWITCH_AND_ABILITY).pressed;
        case TutorialType::DASH_SWITCH_COMBO: {
            return Input::getButton(ButtonType::SWITCH_AND_ABILITY).pressed;
        }
        default: return false;
    }
}

void TutorialObject::getTutorialText(char* buffer, int length) const {
    switch (data.type) {
        case TutorialType::WORLD_SWITCH:
            snprintf(buffer, length, "[%s]: Switch worlds", Menu::getSwitchHelp());
            return;
        case TutorialType::JUMP:
            snprintf(buffer, length, "[%s]: Jump", Menu::getJumpHelp());
            return;
        case TutorialType::DOUBLE_JUMP:
            snprintf(buffer, length, "[%s]: Press in air to double jump", Menu::getJumpHelp());
            return;
        case TutorialType::GLIDER:
            snprintf(buffer, length, "[%s]: Hold to glide", Menu::getAbilityHelp());
            return;
        case TutorialType::DASH:
            snprintf(buffer, length, "[%s]: Use ability", Menu::getAbilityHelp());
            return;
        case TutorialType::WALL_JUMP:
            snprintf(buffer, length, "[%s]: Press while sticking to wall", Menu::getJumpHelp());
            return;
        case TutorialType::DASH_SWITCH_COMBO:
            snprintf(buffer, length, "[%s]: Switch worlds and use ability",
                     Menu::getAbilitySwitchHelp());
            return;
        default: snprintf(buffer, length, "Missing tutorial text");
    }
}
