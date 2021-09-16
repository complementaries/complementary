#include "Font.h"

#include <SDL.h>
#include <SDL_image.h>
#include <array>

#include "graphics/Buffer.h"
#include "graphics/RenderState.h"
#include "graphics/gl/Shader.h"
#include "graphics/gl/Texture.h"
#include "graphics/gl/VertexBuffer.h"

static GL::Shader shader;
static GL::VertexBuffer buffer;
static GL::Texture texture;
static std::array<float, 128> fontWidth;
constexpr int FONT_OFFSET = 32;
constexpr int SYMBOLS_X = 16;
constexpr int SYMBOLS_Y = 8;

bool Font::init() {
    if (shader.compile({"assets/shaders/font.vs", "assets/shaders/font.fs"})) {
        return true;
    }
    buffer.init(GL::VertexBuffer::Attributes().addVector2().addVector2().addRGBA());
    texture.init();
    SDL_Surface* font = IMG_Load("assets/font.png");
    if (font == nullptr) {
        printf("cannot load font: %s\n", IMG_GetError());
        return true;
    }
    Color* data = static_cast<Color*>(font->pixels);
    texture.setData(font->w, font->h, font->pixels);
    fontWidth.fill(0.0f);
    int symbolWidth = font->w / SYMBOLS_X;
    int symbolHeight = font->h / SYMBOLS_Y;
    for (int i = 32; i < 128; i++) {
        int width = 0;
        for (int x = 0; x < symbolWidth; x++) {
            for (int y = 0; y < symbolHeight; y++) {
                int o = i - FONT_OFFSET;
                int base = (o % SYMBOLS_X) * symbolWidth + (o / SYMBOLS_X) * symbolWidth * font->w;
                int index = base + x + y * font->w;
                if ((data[index] & 0xFF) < 160) {
                    width = x + 1;
                }
            }
        }
        fontWidth[i] = static_cast<float>(width) / symbolWidth;
    }
    fontWidth[' '] = 0.25f;
    SDL_FreeSurface(font);
    return false;
}

void Font::prepare() {
    shader.use();
    RenderState::setViewMatrix(shader);
}

void Font::draw(const Vector& pos, float size, Color color, const char* s) {
    texture.bindTo(0);
    Buffer data;
    int index = 0;

    float x = pos.x;
    float y = pos.y;

    while (s[index] != '\0') {
        int c = s[index] & 0x7F;
        float minX = x;
        float minY = y;
        float maxX = minX + size * fontWidth[c];
        float maxY = minY + size;
        float minTexX = (c % SYMBOLS_X) / static_cast<float>(SYMBOLS_X);
        float minTexY = ((c - FONT_OFFSET) / SYMBOLS_X) / static_cast<float>(SYMBOLS_Y);
        float maxTexX = minTexX + 1.0f / SYMBOLS_X * fontWidth[c];
        float maxTexY = minTexY + 1.0f / SYMBOLS_Y;

        data.add(minX).add(minY).add(minTexX).add(minTexY).add(color);
        data.add(maxX).add(minY).add(maxTexX).add(minTexY).add(color);
        data.add(minX).add(maxY).add(minTexX).add(maxTexY).add(color);
        data.add(maxX).add(maxY).add(maxTexX).add(maxTexY).add(color);
        data.add(maxX).add(minY).add(maxTexX).add(minTexY).add(color);
        data.add(minX).add(maxY).add(minTexX).add(maxTexY).add(color);
        x += size * fontWidth[c];
        index++;
    }

    buffer.setData(data.getData(), data.getSize());
    buffer.drawTriangles(6 * index);
}

float Font::getWidth(float size, const char* s) {
    float width = 0.0f;
    int index = 0;
    while (s[index] != '\0') {
        width += fontWidth[s[index] & 0x7F];
        index++;
    }
    width *= size;
    return width;
}