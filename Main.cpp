#include <iostream>

#include <SDL.h>
#include <SDL_image.h>

#define STB_LEAKCHECK_IMPLEMENTATION
#include <stb_leakcheck.h>

int main(int argc, char **argv) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("SDL Example", SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED, 1280, 720, 0);
    if (window == nullptr) {
        fprintf(stderr, "SDL window failed to initialise: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Surface *surface = SDL_GetWindowSurface(window);
    if (surface == nullptr) {
        fprintf(stderr, "Failed to load surface: %s\n", SDL_GetError());
        return 1;
    }

    constexpr int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return 1;
    }

    SDL_Surface *loadedSurface = IMG_Load("assets/image.png");
    SDL_Surface *pngImage = nullptr;
    if (loadedSurface == nullptr) {
        printf("Unable to load image! SDL_image Error: %s\n", IMG_GetError());
        return 1;
    } else {
        pngImage = SDL_ConvertSurface(loadedSurface, surface->format, 0);
        if (pngImage == nullptr) {
            printf("Unable to optimize image! SDL Error: %s\n", SDL_GetError());
            return 1;
        }
        SDL_FreeSurface(loadedSurface);
    }

    SDL_BlitSurface(pngImage, nullptr, surface, nullptr);
    SDL_UpdateWindowSurface(window);

    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_FreeSurface(pngImage);

    SDL_Quit();

    void *memoryLeak = malloc(10);
    stb_leakcheck_dumpmem();
    return 0;
}
