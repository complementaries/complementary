#include <filesystem>
#include <iostream>
#include <switch.h>
#include <sys/socket.h>

#include "graphics/Window.h"

static void setMesaConfig() {
    // Uncomment below to disable error checking and save CPU time (useful for production):
    setenv("MESA_NO_ERROR", "1", 1);

    // Uncomment below to enable Mesa logging:
    setenv("EGL_LOG_LEVEL", "debug", 1);
    setenv("MESA_VERBOSE", "all", 1);
    setenv("NOUVEAU_MESA_DEBUG", "1", 1);

    // Uncomment below to enable shader debugging in Nouveau:
    setenv("NV50_PROG_OPTIMIZE", "0", 1);
    setenv("NV50_PROG_DEBUG", "1", 1);
    setenv("NV50_PROG_CHIPSET", "0x120", 1);
}

int main(int argc, char** args) {
    (void)argc;
    (void)args;

    socketInitializeDefault();
    nxlinkStdio();
    printf("Redirected stdio\n");

    romfsInit();
    chdir("romfs:/");
    setMesaConfig();

#ifndef NDEBUG
    // if (!std::filesystem::exists("assets")) {
    //     puts("Attempted to fix the working directory.\n");
    //     std::filesystem::current_path("..");
    // }
#endif

    if (Window::init()) {
        printf("Returned 1\n");
        socketExit();
        return 1;
    }
    Window::run();
    Window::exit();

    socketExit();
    return 0;
}
