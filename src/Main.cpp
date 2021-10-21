#include <cstring>
#include <filesystem>
#include <iostream>

#include "Arguments.h"
#include "graphics/Window.h"

int main(int argc, char** args) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(args[i], "--muted") == 0) {
            Arguments::muted = true;
        } else if (strcmp(args[i], "--no-vsync") == 0) {
            Arguments::vsync = false;
        } else if (strcmp(args[i], "--skip-anim") == 0) {
            Arguments::skipAnim = true;
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
