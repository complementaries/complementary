#include "Input.h"

#include <cstdio>
#include <cstdlib>

static Button buttons[(size_t)ButtonType::MAX];
static float axes[(size_t)AxisType::MAX];

static float horizontal;
static float joystickFactor = 1.0f;
static bool joystickControlled = false;
static SDL_GameController* controller = nullptr;
static SDL_Haptic* controllerHaptic = nullptr;

void Input::Internal::setButtonPressed(ButtonType type) {
    auto& button = buttons[(size_t)type];
    button.pressed = true;
    button.pressedFirstFrame = true;
    button.pressedTicks = 0;
}

void Input::Internal::setButtonReleased(ButtonType type) {
    auto& button = buttons[(size_t)type];
    button.pressed = false;
    button.pressedFirstFrame = false;
    button.pressedTicks = -1;
}

void Input::Internal::setJoystickFactor(float factor) {
    joystickFactor = factor;
}

void Input::Internal::setJoystickControlled(bool controlled) {
    joystickControlled = controlled;
}

bool Input::Internal::getJoystickControlled() {
    return joystickControlled;
}

void Input::Internal::setAxis(AxisType type, float value) {
    axes[(size_t)type] = value;
}

void Input::Internal::update() {
    for (auto& button : buttons) {
        button.pressedFirstFrame = button.pressed && button.pressedTicks == 0;
        button.pressedTicks = button.pressed ? button.pressedTicks + 1 : 0;
    }
    horizontal = axes[(size_t)AxisType::HORIZONTAL];
    horizontal -= buttons[(size_t)ButtonType::LEFT].pressed * joystickFactor;
    horizontal += buttons[(size_t)ButtonType::RIGHT].pressed * joystickFactor;
}

Button Input::getButton(ButtonType type) {
    return buttons[(size_t)type];
}

float Input::getHorizontal() {
    return horizontal;
}

SDL_GameController* Input::getController() {
    return controller;
}
SDL_Haptic* Input::getControllerHaptic() {
    return controllerHaptic;
}

void Input::setController(SDL_GameController* c) {
    controller = c;
    controllerHaptic = SDL_HapticOpenFromJoystick(SDL_GameControllerGetJoystick(c));
    if (controllerHaptic == nullptr) {
        printf("Warning: Controller does not support haptics! SDL Error: %s\n", SDL_GetError());
    } else {
        if (SDL_HapticRumbleInit(controllerHaptic) < 0) {
            printf("Warning: Unable to initialize rumble! SDL Error: %s\n", SDL_GetError());
        } else {
            printf("Haptic Device Connected!");
        }
    }
}

void Input::closeController() {
    if (controllerHaptic != nullptr) {
        SDL_HapticClose(controllerHaptic);
    }
    if (controller != nullptr) {
        SDL_GameControllerClose(controller);
    }
    controller = nullptr;
    controllerHaptic = nullptr;
}

const char* Input::getButtonName(ButtonType type) {
    switch (type) {
        case ButtonType::JUMP: return "Jump";
        case ButtonType::SWITCH: return "Switch";
        case ButtonType::ABILITY: return "Ability";
        case ButtonType::LEFT: return "Left";
        case ButtonType::RIGHT: return "Right";
        case ButtonType::UP: return "Up";
        case ButtonType::DOWN: return "Down";
        case ButtonType::PAUSE: return "Pause";
        default: return "?";
    }
}
