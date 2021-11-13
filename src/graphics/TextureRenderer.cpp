#include "TextureRenderer.h"

#include <SDL_image.h>
#include <array>
#include <cstdio>

#include "Utils.h"
#include "graphics/Buffer.h"
#include "graphics/RenderState.h"
#include "graphics/Window.h"
#include "graphics/gl/Shader.h"
#include "graphics/gl/Texture.h"
#include "graphics/gl/VertexBuffer.h"
#include "player/Player.h"
#include "tilemap/Tilemap.h"

static GL::Shader shader;
static GL::VertexBuffer buffer;
static GL::Texture abilities;

bool TextureRenderer::init() {
    if (shader.compile({"assets/shaders/texture.vs", "assets/shaders/texture.fs"})) {
        return true;
    }
    buffer.init(GL::VertexBuffer::Attributes().addVector2().addVector2().addRGBA());

    abilities.init();
    SDL_Surface* data = IMG_Load("assets/abilities.png");
    if (data == nullptr) {
        Utils::print("cannot load abilities: %s\n", IMG_GetError());
        return true;
    }
    abilities.setData(data->w, data->h, data->pixels);
    SDL_FreeSurface(data);
    return false;
}

static void renderBox(const Vector& min, const Vector& max, const Vector& tMin, const Vector& tMax,
                      Color c) {
#ifndef NDEBUG
    if (!shader.isBound()) {
        fprintf(stderr, "renderBox on invalid shader\n");
    }
#endif
    static Buffer data;
    data.clear();
    data.add(min.x).add(min.y).add(tMin.x).add(tMin.y).add(c);
    data.add(max.x).add(min.y).add(tMax.x).add(tMin.y).add(c);
    data.add(min.x).add(max.y).add(tMin.x).add(tMax.y).add(c);
    data.add(max.x).add(max.y).add(tMax.x).add(tMax.y).add(c);
    data.add(max.x).add(min.y).add(tMax.x).add(tMin.y).add(c);
    data.add(min.x).add(max.y).add(tMin.x).add(tMax.y).add(c);
    buffer.setStreamData(data.getData(), data.getSize());
    buffer.drawTriangles(6);
}

void TextureRenderer::renderIcon(const Vector& min, const Vector& max, Ability a, int alpha,
                                 float smooth) {
    if (a == Ability::NONE) {
        return;
    }
    shader.use();
    shader.setFloat("smoothing", smooth);
    RenderState::setViewMatrix(shader);
    abilities.bindTo();
    int id = static_cast<int>(a) - 1;
    Vector tMin((id % 2) * 0.5f, (id / 2) * 0.5f);
    renderBox(min, max, tMin, tMin + Vector(0.5f, 0.5f),
              ColorUtils::setAlpha(AbilityUtils::getColor(a), alpha));
}

void TextureRenderer::render(float lag) {
    (void)lag;
    Vector wSize(Tilemap::getWidth(), Tilemap::getHeight());
    Vector size(4.0f, 4.0f);
    float smooth = std::min(0.3f * 1500.0f / Window::getWidth(), 0.95f);
    renderIcon(wSize - size, wSize, Player::getAbility(), 255, smooth);
    renderIcon(wSize - Vector(size.x * 0.5f, size.y * 1.5f), wSize - Vector(0.0, size.y),
               Player::getPassiveAbility(), 100, smooth);
}
