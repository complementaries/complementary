#include "Window.h"

#include <chrono>
#include <iostream>

#include <SDL.h>
#include <algorithm>
#include <cmath>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>

#include "Arguments.h"
#include "Game.h"
#include "Input.h"
#include "Profiler.h"
#include "Utils.h"
#include "graphics/gl/Glew.h"
#include "objects/Objects.h"
#include "sound/SoundManager.h"

typedef long long int Nanos;
static constexpr Nanos NANOS_PER_TICK = 1'000'000'000L * Window::SECONDS_PER_TICK;

static SDL_Window* window = nullptr;
static bool running = false;
static int width = 850;
static int height = 480;
static int previousWidth = 850;
static int previousHeight = 480;
static bool fullscreen = false;

bool Window::init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        Utils::printError("SDL failed to initialise: %s\n", SDL_GetError());
        return true;
    }
    if (!Arguments::muted) {
        if (SDL_Init(SDL_INIT_AUDIO) != 0) {
            Utils::printError("SDL audio failed to initialise: %s\n", SDL_GetError());
            Arguments::muted = true;
        }
    }

    if (SoundManager::init()) {
        Utils::printError("Disabled audio due to sound manager init failure\n");
    }

    if (SoundManager::loadSounds()) {
        return true;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow("Complementary", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              width, height,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (window == nullptr) {
        Utils::printError("SDL window failed to initialise: %s\n", SDL_GetError());
        return true;
    }

    SDL_GLContext gContext = SDL_GL_CreateContext(window);
    if (gContext == nullptr) {
        Utils::printError("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
        return true;
    }

    glEnable(GL_MULTISAMPLE);

    std::cout << glGetString(GL_VERSION) << "\n";

    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        Utils::printError("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
        return true;
    }

    if (SDL_GL_SetSwapInterval(Arguments::vsync) < 0) {
        Utils::printError("unable to set swap interval to %d\n", Arguments::vsync);
    }

    if (Arguments::samples > 1) {
        if (SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1) < 0 ||
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, Arguments::samples) < 0) {
            Utils::printError("Failed to enable multisampling.\n");
        }
    }

    if (SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1) < 0) {
        Utils::printError("Failed to enable GPU acceleration.\n");
    }

    // Imgui setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // TODO: keyboard and gamepad navigation is disabled because it
    // conflicts with player movement, figure something out
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad; //
    // Enable Keyboard and Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    if (!ImGui_ImplSDL2_InitForOpenGL(window, gContext)) {
        Utils::printError("Failed to initialize ImGui");
        return true;
    }
    if (!ImGui_ImplOpenGL3_Init("#version 410")) {
        Utils::printError("Failed to initialize ImGui");
        return true;
    }

    // Initialize game controller subsystem
    if (SDL_WasInit(SDL_INIT_JOYSTICK) != 1) SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    if (SDL_WasInit(SDL_INIT_GAMECONTROLLER) != 1) SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);
    if (SDL_WasInit(SDL_INIT_HAPTIC) != 1) SDL_InitSubSystem(SDL_INIT_HAPTIC);
    SDL_GameControllerEventState(SDL_ENABLE);

    return false;
}

static void resize(int w, int h) {
    Game::onWindowResize(w, h);
    width = w;
    height = h;
}

static void resize() {
    int w, h;
    SDL_GL_GetDrawableSize(window, &w, &h);
    resize(w, h);
}

static void toggleFullscreen() {
    if (!fullscreen) {
        previousHeight = height;
        previousWidth = width;

        SDL_DisplayMode mode = {};
        int displayIndex = SDL_GetWindowDisplayIndex(window);
        if (displayIndex < 0) {
            Utils::printError("%s\n", SDL_GetError());
            return;
        }
        if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) < 0) {
            Utils::printError("%s\n", SDL_GetError());
            return;
        }
        if (SDL_GetDesktopDisplayMode(displayIndex, &mode) < 0) {
            Utils::printError("%s\n", SDL_GetError());
            return;
        }

        float hdpi, vdpi;
        if (SDL_GetDisplayDPI(displayIndex, nullptr, &hdpi, &vdpi) < 0) {
            Utils::printError("%s\n", SDL_GetError());
            hdpi = 1.f;
            vdpi = 1.f;
        }

        SDL_SetWindowSize(window, mode.w, mode.h);
        resize(mode.w * hdpi, mode.h * vdpi);
    } else {
        if (SDL_SetWindowFullscreen(window, 0) < 0) {
            Utils::printError("%s\n", SDL_GetError());
            return;
        }
        SDL_SetWindowSize(window, previousWidth, previousHeight);
        resize();
    }

    fullscreen = !fullscreen;
}

static void pollEvents() {
    SDL_Event e;

    while (SDL_PollEvent(&e)) {
        ImGui_ImplSDL2_ProcessEvent(&e);

        switch (e.type) {
            case SDL_QUIT: {
                running = false;
                break;
            }
            case SDL_KEYDOWN: {
                if (!e.key.repeat) {
                    switch (e.key.keysym.sym) {
                        case SDLK_SPACE: {
                            Input::Internal::setButtonPressed(ButtonType::JUMP);
                            break;
                        }
                        case SDLK_RETURN: {
                            Input::Internal::setButtonPressed(ButtonType::SWITCH);
                            break;
                        }
                        case SDLK_RSHIFT:
                            Input::Internal::setButtonPressed(ButtonType::SWITCH);
                            [[fallthrough]];
                        case SDLK_LSHIFT:
                            Input::Internal::setButtonPressed(ButtonType::ABILITY);
                            break;
                        case SDLK_LEFT:
                        case SDLK_a: Input::Internal::setButtonPressed(ButtonType::LEFT); break;
                        case SDLK_RIGHT:
                        case SDLK_d: Input::Internal::setButtonPressed(ButtonType::RIGHT); break;
                        case SDLK_UP:
                        case SDLK_w: Input::Internal::setButtonPressed(ButtonType::UP); break;
                        case SDLK_DOWN:
                        case SDLK_s: Input::Internal::setButtonPressed(ButtonType::DOWN); break;
                        case SDLK_m: SoundManager::mute(); break;
                        case SDLK_p: Input::Internal::setButtonPressed(ButtonType::PAUSE); break;
                        case SDLK_ESCAPE:
                            Input::Internal::setButtonPressed(ButtonType::PAUSE);
                            break;
                    }
                }
                break;
            }
            case SDL_KEYUP: {
                switch (e.key.keysym.sym) {
                    case SDLK_SPACE: Input::Internal::setButtonReleased(ButtonType::JUMP); break;
                    case SDLK_RETURN: Input::Internal::setButtonReleased(ButtonType::SWITCH); break;
                    case SDLK_RSHIFT:
                        Input::Internal::setButtonReleased(ButtonType::SWITCH);
                        [[fallthrough]];
                    case SDLK_LSHIFT:
                        Input::Internal::setButtonReleased(ButtonType::ABILITY);
                        break;
                    case SDLK_LEFT:
                    case SDLK_a: Input::Internal::setButtonReleased(ButtonType::LEFT); break;
                    case SDLK_RIGHT:
                    case SDLK_d: Input::Internal::setButtonReleased(ButtonType::RIGHT); break;
                    case SDLK_UP:
                    case SDLK_w: Input::Internal::setButtonReleased(ButtonType::UP); break;
                    case SDLK_DOWN:
                    case SDLK_s: Input::Internal::setButtonReleased(ButtonType::DOWN); break;
                    case SDLK_p: Input::Internal::setButtonReleased(ButtonType::PAUSE); break;
                    case SDLK_F11: toggleFullscreen(); break;
                }
                break;
            }
            case SDL_CONTROLLERBUTTONDOWN: {
                switch (e.cbutton.button) {
                    case SDL_CONTROLLER_BUTTON_A:
                    case SDL_CONTROLLER_BUTTON_B:
                        Input::Internal::setButtonPressed(ButtonType::JUMP);
                        break;
                    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
                    case SDL_CONTROLLER_BUTTON_X:
                        Input::Internal::setButtonPressed(ButtonType::ABILITY);
                        break;
                    case SDL_CONTROLLER_BUTTON_Y:
                        Input::Internal::setButtonPressed(ButtonType::SWITCH);
                        Input::Internal::setButtonPressed(ButtonType::ABILITY);
                        break;
                    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
                        Input::Internal::setButtonPressed(ButtonType::SWITCH);
                        break;
                    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                        Input::Internal::setButtonPressed(ButtonType::LEFT);
                        break;
                    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                        Input::Internal::setButtonPressed(ButtonType::RIGHT);
                        break;
                    case SDL_CONTROLLER_BUTTON_DPAD_UP:
                        Input::Internal::setButtonPressed(ButtonType::UP);
                        break;
                    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                        Input::Internal::setButtonPressed(ButtonType::DOWN);
                        break;
                    case SDL_CONTROLLER_BUTTON_START:
                        Input::Internal::setButtonPressed(ButtonType::PAUSE);
                        break;
                }
                break;
            }
            case SDL_CONTROLLERBUTTONUP: {
                switch (e.cbutton.button) {
                    case SDL_CONTROLLER_BUTTON_A:
                    case SDL_CONTROLLER_BUTTON_B:
                        Input::Internal::setButtonReleased(ButtonType::JUMP);
                        break;
                    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
                    case SDL_CONTROLLER_BUTTON_X:
                        Input::Internal::setButtonReleased(ButtonType::ABILITY);
                        break;
                    case SDL_CONTROLLER_BUTTON_Y:
                        Input::Internal::setButtonReleased(ButtonType::SWITCH);
                        Input::Internal::setButtonReleased(ButtonType::ABILITY);
                        break;
                    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
                        Input::Internal::setButtonReleased(ButtonType::SWITCH);
                        break;
                    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                        Input::Internal::setButtonReleased(ButtonType::LEFT);
                        break;
                    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                        Input::Internal::setButtonReleased(ButtonType::RIGHT);
                        break;
                    case SDL_CONTROLLER_BUTTON_DPAD_UP:
                        Input::Internal::setButtonReleased(ButtonType::UP);
                        break;
                    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                        Input::Internal::setButtonReleased(ButtonType::DOWN);
                        break;
                    case SDL_CONTROLLER_BUTTON_START:
                        Input::Internal::setButtonReleased(ButtonType::PAUSE);
                        break;
                }
                break;
            }
            case SDL_CONTROLLERAXISMOTION: {
                float value = e.caxis.value / 32768.0f;
                int sign = value < 0 ? -1 : 1;
                switch (e.jaxis.axis) {
                    case SDL_CONTROLLER_AXIS_LEFTX: {
                        Input::Internal::setJoystickFactor(1.0f);
                        value = (std::abs(value) - 0.2f) / (0.9f - 0.2f);
                        value = (float)sign * std::clamp(value, 0.0f, 1.0f);
                        if (value < 0) {
                            Input::Internal::setJoystickFactor(std::abs(value));
                            Input::Internal::setButtonPressed(ButtonType::LEFT);
                            Input::Internal::setButtonReleased(ButtonType::RIGHT);
                            Input::Internal::setJoystickControlled(true);
                        } else if (value > 0) {
                            Input::Internal::setJoystickFactor(std::abs(value));
                            Input::Internal::setButtonPressed(ButtonType::RIGHT);
                            Input::Internal::setButtonReleased(ButtonType::LEFT);
                            Input::Internal::setJoystickControlled(true);
                        } else if (Input::Internal::getJoystickControlled() == true) {
                            Input::Internal::setJoystickControlled(false);
                            Input::Internal::setButtonReleased(ButtonType::LEFT);
                            Input::Internal::setButtonReleased(ButtonType::RIGHT);
                        }
                        break;
                    }
                    case SDL_CONTROLLER_AXIS_TRIGGERLEFT: {
                        if (value > 0.5f && !Input::getButton(ButtonType::ABILITY).pressed) {
                            Input::Internal::setButtonPressed(ButtonType::ABILITY);
                        } else if (value <= 0.5f) {
                            Input::Internal::setButtonReleased(ButtonType::ABILITY);
                        }
                        break;
                    }
                    case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: {
                        if (value > 0.5f && !Input::getButton(ButtonType::SWITCH).pressed) {
                            Input::Internal::setButtonPressed(ButtonType::SWITCH);
                        } else if (value <= 0.5f) {
                            Input::Internal::setButtonReleased(ButtonType::SWITCH);
                        }
                        break;
                    }
                }
                break;
            }
            case SDL_CONTROLLERDEVICEADDED: {
                if (Input::getController() == NULL) {
                    int nJoysticks = SDL_NumJoysticks();

                    bool gamePadConnected = false;
                    for (int i = 0; i < nJoysticks && !gamePadConnected; i++) {
                        if (SDL_IsGameController(i)) {
                            gamePadConnected = true;
                        }
                    }
                    if (gamePadConnected) {
                        // Open the controller and add it to our list
                        SDL_GameController* pad = SDL_GameControllerOpen(0);
                        if (SDL_GameControllerGetAttached(pad) == 1) {
                            Input::setController(pad);
                            // std::cout << SDL_GameControllerMapping(Input::getController());
                            // If we want to change the mapping, this is how it works
                            // SDL_GameControllerAddMapping("0,PS4 Controller, a:b11, b:b10");
                        } else {
                            std::cout << "SDL_GetError() = " << SDL_GetError() << std::endl;
                        }
                    }
                }
                break;
            }
            case SDL_CONTROLLERDEVICEREMOVED: {
                if (Input::getController() != NULL &&
                    SDL_GameControllerFromInstanceID(e.adevice.which) == Input::getController()) {
                    Input::closeController();
                }
                break;
            }
            case SDL_WINDOWEVENT: {
                if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                    resize();
                }
                break;
            }
            case SDL_MOUSEMOTION:
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEWHEEL: {
                Game::onMouseEvent(&e);
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
    if (Arguments::muted) {
        SoundManager::mute();
    }

    running = true;
    Nanos lag = 0;
    Nanos lastTime = getNanos();
    while (running) {
        {
#ifndef NDEBUG
            Profiler::Timer timer(Profiler::eventPollNanos);
#endif
            pollEvents();
        }

        Nanos time = getNanos();
        lag += time - lastTime;
        lastTime = time;
        int i = 0;
        while (lag >= NANOS_PER_TICK) {
            lag -= NANOS_PER_TICK;
            Input::Internal::update();
            Game::tick();
            i++;
            if (i > 5) {
                int skip = lag / NANOS_PER_TICK;
                lag -= skip * NANOS_PER_TICK;
                fprintf(stderr, "Lagging, skipped %d ticks\n", skip);
            }
        }

        Game::render(static_cast<float>(lag) / NANOS_PER_TICK);
        {
#ifndef NDEBUG
            Profiler::Timer timer(Profiler::postGameRenderNanos);
#endif
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(window);
            ImGui::NewFrame();
#ifndef NDEBUG
            Game::renderImGui();
#endif
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
        {
#ifndef NDEBUG
            Profiler::Timer timer(Profiler::bufferSwapNanos);
#endif
            SDL_GL_SwapWindow(window);
        }
    }

    Input::closeController();
    Objects::clear();
    Objects::clearPrototypes();
    SDL_DestroyWindow(window);
    SDL_Quit();
    SoundManager::quit();
}

void Window::exit() {
    running = false;
}

int Window::getWidth() {
    return width;
}

int Window::getHeight() {
    return height;
}
