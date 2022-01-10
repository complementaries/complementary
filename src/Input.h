#ifndef INPUT_H
#define INPUT_H

#include <SDL_gamecontroller.h>
#include <SDL_haptic.h>
#include <string>

enum class ButtonType {
    JUMP,
    SWITCH,
    ABILITY,
    SWITCH_AND_ABILITY,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    PAUSE,
    CONFIRM,

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
    void reset() {
        pressedTicks = 0;
        pressed = false;
        pressedFirstFrame = false;
    }
};

namespace Input {
    namespace Internal {
        void setButtonPressed(ButtonType type);
        void setButtonReleased(ButtonType type);
        void setJoystickFactor(float factor);
        void setJoystickControlled(bool controlled);
        bool getJoystickControlled();
        void setAxis(AxisType type, float value);
        void update();
    }

    Button& getButton(ButtonType type);
    float getHorizontal();
    SDL_GameController* getController();
    void setController(SDL_GameController* controller);
    const char* getButtonName(ButtonType button);
    void closeController();
    SDL_Haptic* getControllerHaptic();
    void playRumble(float strength, uint32_t length);
}

#endif
