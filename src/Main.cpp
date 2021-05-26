#include <filesystem>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>

#include "graphics/Window.h"

int main(int argc, char** args) {
    (void)argc;
    (void)args;

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
