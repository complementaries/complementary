#ifndef INPUT_H
#define INPUT_H

#include <SDL_gamecontroller.h>

enum class ButtonType {
    JUMP,
    SWITCH,
    ABILITY,
    LEFT,
    RIGHT,

    MAX
};

enum class AxisType {
    HORIZONTAL,

    MAX
};

struct Button {
    int pressedTicks;
    bool pressed;
    bool pressedFirstFrame;
};

namespace Input {
    namespace Internal {
        void setButtonPressed(ButtonType type);
        void setButtonReleased(ButtonType type);
        void setAxis(AxisType type, float value);
        void update();
    }

    Button getButton(ButtonType type);
    float getHorizontal();
    SDL_GameController* getController();
    void setController(SDL_GameController* controller);
    static SDL_GameController* controller;
    static bool buttonPressed[SDL_CONTROLLER_BUTTON_MAX];
}

#endif
