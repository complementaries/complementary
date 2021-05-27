#include "Window.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL.h>
#include <chrono>
#include <iostream>

#include "Game.h"
#include "Input.h"

typedef long long int Nanos;
static constexpr Nanos NANOS_PER_TICK = 1'000'000'000L * Window::SECONDS_PER_TICK;

static SDL_Window* window = nullptr;
static bool running = false;

bool Window::init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
        return true;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow("Complementary", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              850, 480, SDL_WINDOW_OPENGL);
    if (window == nullptr) {
        fprintf(stderr, "SDL window failed to initialise: %s\n", SDL_GetError());
        return true;
    }
    SDL_MinimizeWindow(window);

    SDL_GLContext gContext = SDL_GL_CreateContext(window);
    if (gContext == nullptr) {
        fprintf(stderr, "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
        return true;
    }

    std::cout << glGetString(GL_VERSION) << "\n";

    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        fprintf(stderr, "Error initializing GLEW! %s\n", glewGetErrorString(glewError));
        return true;
    }

    if (SDL_GL_SetSwapInterval(1) < 0) {
        fprintf(stderr, "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
    }

    return false;
}

static void pollEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT: {
                running = false;
                break;
            }
            case SDL_KEYDOWN: {
                if (!e.key.repeat) {
                    switch (e.key.keysym.sym) {
                        case SDLK_SPACE: Input::Internal::setButtonPressed(ButtonType::JUMP); break;
                        case SDLK_RETURN:
                            Input::Internal::setButtonPressed(ButtonType::SWITCH);
                            break;
                        case SDLK_LSHIFT:
                        case SDLK_RSHIFT:
                            Input::Internal::setButtonPressed(ButtonType::ABILITY);
                            break;
                        case SDLK_LEFT:
                        case SDLK_a: Input::Internal::setButtonPressed(ButtonType::LEFT); break;
                        case SDLK_RIGHT:
                        case SDLK_d: Input::Internal::setButtonPressed(ButtonType::RIGHT); break;
                    }
                }
                break;
            }
            case SDL_KEYUP: {
                switch (e.key.keysym.sym) {
                    case SDLK_SPACE: Input::Internal::setButtonReleased(ButtonType::JUMP); break;
                    case SDLK_RETURN: Input::Internal::setButtonReleased(ButtonType::SWITCH); break;
                    case SDLK_LSHIFT:
                    case SDLK_RSHIFT:
                        Input::Internal::setButtonReleased(ButtonType::ABILITY);
                        break;
                    case SDLK_LEFT:
                    case SDLK_a: Input::Internal::setButtonReleased(ButtonType::LEFT); break;
                    case SDLK_RIGHT:
                    case SDLK_d: Input::Internal::setButtonReleased(ButtonType::RIGHT); break;
                }
                break;
            }
        }
    }
}

static Nanos getNanos() {
    using namespace std::chrono;
    return duration_cast<nanoseconds>(high_resolution_clock::now().time_since_epoch()).count();
}

void Window::run() {
    if (Game::init()) {
        return;
    }
    running = true;
    Nanos lag = 0;
    Nanos lastTime = getNanos();
    while (running) {
        pollEvents();

        Nanos time = getNanos();
        lag += time - lastTime;
        lastTime = time;
        while (lag >= NANOS_PER_TICK) {
            lag -= NANOS_PER_TICK;
            Input::Internal::update();
            Game::tick();
        }

        Game::render(static_cast<float>(lag) / NANOS_PER_TICK);
        SDL_GL_SwapWindow(window);
    }
}

void Window::exit() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}
