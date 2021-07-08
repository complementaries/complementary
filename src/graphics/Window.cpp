#include "Window.h"

#include <chrono>
#include <iostream>

#include <SDL.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>

#include "Game.h"
#include "Input.h"
#include "graphics/gl/Glew.h"

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
        fprintf(stderr, "Failed to initialize ImGui");
        return true;
    }
    if (!ImGui_ImplOpenGL3_Init("#version 410")) {
        fprintf(stderr, "Failed to initialize ImGui");
        return true;
    }

    // Initialize game controller subsystem
    if (SDL_WasInit(SDL_INIT_GAMECONTROLLER) != 1) SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);
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
    SDL_GameControllerEventState(SDL_ENABLE);
    return false;
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
            case SDL_CONTROLLERBUTTONDOWN: {
                switch (e.cbutton.button) {
                    case SDL_CONTROLLER_BUTTON_A:
                        Input::Internal::setButtonPressed(ButtonType::JUMP);
                        Input::buttonPressed[SDL_CONTROLLER_BUTTON_A] = true;
                        break;
                    case SDL_CONTROLLER_BUTTON_B:
                        Input::Internal::setButtonPressed(ButtonType::JUMP);
                        Input::buttonPressed[SDL_CONTROLLER_BUTTON_B] = true;
                        break;
                    case SDL_CONTROLLER_BUTTON_X:
                        Input::Internal::setButtonPressed(ButtonType::ABILITY);
                        Input::buttonPressed[SDL_CONTROLLER_BUTTON_X] = true;
                        break;
                    case SDL_CONTROLLER_BUTTON_Y:
                        Input::Internal::setButtonPressed(ButtonType::ABILITY);
                        Input::buttonPressed[SDL_CONTROLLER_BUTTON_Y] = true;
                        break;
                    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
                        Input::Internal::setButtonPressed(ButtonType::SWITCH);
                        Input::buttonPressed[SDL_CONTROLLER_BUTTON_LEFTSHOULDER] = true;
                        break;
                    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
                        Input::Internal::setButtonPressed(ButtonType::SWITCH);
                        Input::buttonPressed[SDL_CONTROLLER_BUTTON_RIGHTSHOULDER] = true;
                        break;
                    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                        Input::Internal::setButtonPressed(ButtonType::LEFT);
                        Input::buttonPressed[SDL_CONTROLLER_BUTTON_DPAD_LEFT] = true;
                        break;
                    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                        Input::Internal::setButtonPressed(ButtonType::RIGHT);
                        Input::buttonPressed[SDL_CONTROLLER_BUTTON_DPAD_RIGHT] = true;
                        break;
                }
            }
            case SDL_CONTROLLERBUTTONUP: {
                switch (e.cbutton.button) {
                    case SDL_CONTROLLER_BUTTON_A:
                        if (Input::buttonPressed[SDL_CONTROLLER_BUTTON_A]) {
                            Input::buttonPressed[SDL_CONTROLLER_BUTTON_A] = false;
                        } else {
                            Input::Internal::setButtonReleased(ButtonType::JUMP);
                        }
                        break;
                    case SDL_CONTROLLER_BUTTON_B:
                        if (Input::buttonPressed[SDL_CONTROLLER_BUTTON_B]) {
                            Input::buttonPressed[SDL_CONTROLLER_BUTTON_B] = false;
                        } else {
                            Input::Internal::setButtonReleased(ButtonType::JUMP);
                        }
                        break;
                    case SDL_CONTROLLER_BUTTON_X:
                        if (Input::buttonPressed[SDL_CONTROLLER_BUTTON_X]) {
                            Input::buttonPressed[SDL_CONTROLLER_BUTTON_X] = false;
                        } else {
                            Input::Internal::setButtonReleased(ButtonType::ABILITY);
                        }
                        break;
                    case SDL_CONTROLLER_BUTTON_Y:
                        if (Input::buttonPressed[SDL_CONTROLLER_BUTTON_Y]) {
                            Input::buttonPressed[SDL_CONTROLLER_BUTTON_Y] = false;
                        } else {
                            Input::Internal::setButtonReleased(ButtonType::ABILITY);
                        }
                        break;
                    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
                        if (Input::buttonPressed[SDL_CONTROLLER_BUTTON_LEFTSHOULDER]) {
                            Input::buttonPressed[SDL_CONTROLLER_BUTTON_LEFTSHOULDER] = false;
                        } else {
                            Input::Internal::setButtonReleased(ButtonType::SWITCH);
                        }
                        break;
                    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
                        if (Input::buttonPressed[SDL_CONTROLLER_BUTTON_RIGHTSHOULDER]) {
                            Input::buttonPressed[SDL_CONTROLLER_BUTTON_RIGHTSHOULDER] = false;
                        } else {
                            Input::Internal::setButtonReleased(ButtonType::SWITCH);
                        }
                        break;
                    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                        if (Input::buttonPressed[SDL_CONTROLLER_BUTTON_DPAD_LEFT]) {
                            Input::buttonPressed[SDL_CONTROLLER_BUTTON_DPAD_LEFT] = false;
                        } else {
                            Input::Internal::setButtonReleased(ButtonType::LEFT);
                        }
                        break;
                    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                        if (Input::buttonPressed[SDL_CONTROLLER_BUTTON_DPAD_RIGHT]) {
                            Input::buttonPressed[SDL_CONTROLLER_BUTTON_DPAD_RIGHT] = false;
                        } else {
                            Input::Internal::setButtonReleased(ButtonType::RIGHT);
                        }
                        break;
                }
            }
                // TODO: make this work
                // case SDL_CONTROLLERAXISMOTION: {
                //    switch (e.jaxis.axis) {
                //        case SDL_CONTROLLER_AXIS_LEFTX:
                //            if (e.caxis.value < -2000) {
                //                if (!Input::buttonPressed[SDL_CONTROLLER_BUTTON_LEFTSTICK]) {
                //                    std::cout << "Left" << std::endl;
                //                    Input::Internal::setButtonPressed(ButtonType::LEFT);
                //                    Input::buttonPressed[SDL_CONTROLLER_BUTTON_LEFTSTICK] = true;
                //                    Input::buttonPressed[SDL_CONTROLLER_BUTTON_RIGHTSTICK] =
                //                    false;
                //                }
                //            } else if (e.caxis.value > 2000) {
                //                if (!Input::buttonPressed[SDL_CONTROLLER_BUTTON_RIGHTSTICK]) {
                //                    std::cout << "Right" << std::endl;
                //                    Input::Internal::setButtonPressed(ButtonType::RIGHT);
                //                    Input::buttonPressed[SDL_CONTROLLER_BUTTON_RIGHTSTICK] = true;
                //                    Input::buttonPressed[SDL_CONTROLLER_BUTTON_LEFTSTICK] = false;
                //                }
                //            } else {
                //                Input::Internal::setButtonReleased(ButtonType::LEFT);
                //                Input::Internal::setButtonReleased(ButtonType::RIGHT);
                //                Input::buttonPressed[SDL_CONTROLLER_BUTTON_LEFTSTICK] = false;
                //                Input::buttonPressed[SDL_CONTROLLER_BUTTON_RIGHTSTICK] = false;
                //            }
                //            break;
                //    }
                //}

                // TODO: make sure controllers can be connected and disconnected during gameplay
                // case SDL_CONTROLLERDEVICEADDED: {
                //}
                // case SDL_CONTROLLERDEVICEREMOVED: {
                //}
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

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

#ifndef NDEBUG
        Game::renderImGui();
#endif

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    }
}

void Window::exit() {
    if (Input::getController() != NULL) {
        SDL_GameControllerClose(Input::getController());
    }
    Input::setController(nullptr);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
