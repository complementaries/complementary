#include <iostream>

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_opengl.h>

#define STB_LEAKCHECK_IMPLEMENTATION
#include <stb_leakcheck.h>

static const GLchar *vertexShaderCode =
    "#version 410\n"
    "layout(location = 0) in vec2 pos;"
    ""
    "void main(void) {"
    "    gl_Position = vec4(pos, 0.0, 1.0);"
    "}";

static const GLchar *fragmentShaderCode =
    "#version 410\n"
    ""
    "uniform sampler2D samp;"
    ""
    "out vec4 color;"
    ""
    "void main(void) {"
    "    color = vec4(1.0, 0.0, 0.0, 1.0);"
    "}";

static GLuint vertexShader = 0;
static GLuint fragmentShader = 0;
static GLuint program = 0;
static GLuint vertexArray = 0;
static GLuint vertexBuffer = 0;

static bool checkShaderError(GLuint shader, const char *name) {
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        printf("cannot compile %s shader:\n", name);
        GLchar error[256];
        glGetShaderInfoLog(shader, 256, NULL, error);
        puts(error);
        return true;
    }
    return false;
}

static bool initShaders() {
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
    glCompileShader(vertexShader);
    if (checkShaderError(vertexShader, "vertex")) {
        return true;
    }
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
    glCompileShader(fragmentShader);
    if (checkShaderError(fragmentShader, "fragment")) {
        return true;
    }

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        puts("cannot link program:");
        GLchar error[256];
        glGetProgramInfoLog(program, 256, NULL, error);
        puts(error);
        return true;
    }
    glUseProgram(program);
    return false;
}

static void initVertexBuffer() {
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(GLfloat) * 2, NULL);
    glEnableVertexAttribArray(0);

    GLfloat data[] = {-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f};
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
}

static bool init() {
    if (initShaders()) {
        return true;
    }
    initVertexBuffer();
    return false;
}

static void cleanUp() {
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vertexArray);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteProgram(program);
}

int main(int argc, char **argv) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window *window = SDL_CreateWindow("SDL Example", SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_OPENGL);
    if (window == nullptr) {
        fprintf(stderr, "SDL window failed to initialise: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GLContext gContext = SDL_GL_CreateContext(window);
    if (gContext == nullptr) {
        fprintf(stderr, "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        fprintf(stderr, "Error initializing GLEW! %s\n", glewGetErrorString(glewError));
        return 1;
    }

    if (SDL_GL_SetSwapInterval(1) < 0) {
        fprintf(stderr, "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
    }

    if (init()) {
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
        SDL_GL_SwapWindow(window);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(vertexArray);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    cleanUp();

    SDL_DestroyWindow(window);
    SDL_FreeSurface(pngImage);

    SDL_Quit();

    void *memoryLeak = malloc(10);
    stb_leakcheck_dumpmem();
    return 0;
}
