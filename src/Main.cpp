#include <cstring>
#include <filesystem>
#include <iostream>

#include "Arguments.h"
#include "graphics/Window.h"

int main(int argc, char** args) {
    int parseIndex = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(args[i], "--muted") == 0) {
            Arguments::muted = true;
        } else if (strcmp(args[i], "--no-vsync") == 0) {
            Arguments::vsync = false;
        } else if (strcmp(args[i], "--skip-anim") == 0) {
            Arguments::skipAnim = true;
        } else if (parseIndex == 0) {
            int samples = atoi(args[i]);
            if (samples <= 0) {
                std::cout << "passed invalid number of samples, using default\n";
            } else {
                Arguments::samples = samples;
            }
            parseIndex++;
        }
    }

    std::cout << "running in " << std::filesystem::current_path() << "\n";

#ifndef NDEBUG
    if (!std::filesystem::exists("assets")) {
        puts("Attempted to fix the working directory.\n");
        std::filesystem::current_path("..");
    }
#endif

    if (Window::init()) {
        return 1;
    }
    Window::run();
    Window::exit();
    return 0;
}
