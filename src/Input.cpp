#include "Input.h"

#include <cstdio>
#include <cstdlib>

static Button buttons[(size_t)ButtonType::MAX];
static float axes[(size_t)AxisType::MAX];

static float horizontal;

void Input::Internal::setButtonPressed(ButtonType type) {
    auto& button = buttons[(size_t)type];
    button.pressed = true;
    button.pressedFirstFrame = true;
    button.pressedTicks = -1;
}

void Input::Internal::setButtonReleased(ButtonType type) {
    auto& button = buttons[(size_t)type];
    button.pressed = false;
    button.pressedFirstFrame = false;
    button.pressedTicks = -1;
}

void Input::Internal::setAxis(AxisType type, float value) {
    axes[(size_t)type] = value;
}

void Input::Internal::update() {
    for (auto& button : buttons) {
        button.pressedTicks++;
    }

    horizontal = axes[(size_t)AxisType::HORIZONTAL];
    horizontal -= buttons[(size_t)ButtonType::LEFT].pressed;
    horizontal += buttons[(size_t)ButtonType::RIGHT].pressed;
}

Button Input::getButton(ButtonType type) {
    return buttons[(size_t)type];
}

float Input::getHorizontal() {
    return horizontal;
}