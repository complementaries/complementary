#include <iostream>

#include <SDL.h>
#include <SDL_image.h>

#define STB_LEAKCHECK_IMPLEMENTATION
#include <stb_leakcheck.h>

int main(int argc, char **argv) {
    /*
     * Initialises the SDL video subsystem (as well as the events subsystem).
     * Returns 0 on success or a negative error code on failure using SDL_GetError().
     */
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
        return 1;
    }

    /* Creates a SDL window */
    auto window = SDL_CreateWindow("SDL Example",           /* Title of the SDL window */
                                   SDL_WINDOWPOS_UNDEFINED, /* Position x of the window */
                                   SDL_WINDOWPOS_UNDEFINED, /* Position y of the window */
                                   1280,                    /* Width of the window in pixels */
                                   720,                     /* Height of the window in pixels */
                                   0);                      /* Additional flag(s) */

    /* Checks if window has been created; if not, exits program */
    if (!window) {
        fprintf(stderr, "SDL window failed to initialise: %s\n", SDL_GetError());
        return 1;
    }

    auto surface = SDL_GetWindowSurface(window);
    if (!surface) {
        fprintf(stderr, "Failed to load surface: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return 1;
    }

    auto loadedSurface = IMG_Load("image.png");
    SDL_Surface *pngImage;
    if (loadedSurface == nullptr) {
        printf("Unable to load image! SDL_image Error: %s\n", IMG_GetError());
        return 1;
    } else {
        // Convert surface to screen format
        pngImage = SDL_ConvertSurface(loadedSurface, surface->format, 0);
        if (pngImage == nullptr) {
            printf("Unable to optimize image! SDL Error: %s\n", SDL_GetError());
            return 1;
        }

        // Get rid of old loaded surface
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

    /* Frees memory */
    SDL_DestroyWindow(window);
    SDL_FreeSurface(pngImage);

    /* Shuts down all SDL subsystems */
    SDL_Quit();

    void *memoryLeak = malloc(10);
    stb_leakcheck_dumpmem();

    return 0;
}
