#include "TextureRenderer.h"

#include <SDL_image.h>
#include <array>
#include <cstdio>

#include "graphics/Buffer.h"
#include "graphics/RenderState.h"
#include "graphics/gl/Shader.h"
#include "graphics/gl/Texture.h"
#include "graphics/gl/VertexBuffer.h"
#include "player/Player.h"

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
        printf("cannot load abilities: %s\n", IMG_GetError());
        return true;
    }
    abilities.setData(data->w, data->h, data->pixels);
    SDL_FreeSurface(data);
    return false;
}

static void renderBox(const Vector& min, const Vector& max, const Vector& tMin, const Vector& tMax,
                      Color c) {
    Buffer data;
    data.add(min.x).add(min.y).add(tMin.x).add(tMin.y).add(c);
    data.add(max.x).add(min.y).add(tMax.x).add(tMin.y).add(c);
    data.add(min.x).add(max.y).add(tMin.x).add(tMax.y).add(c);
    data.add(max.x).add(max.y).add(tMax.x).add(tMax.y).add(c);
    data.add(max.x).add(min.y).add(tMax.x).add(tMin.y).add(c);
    data.add(min.x).add(max.y).add(tMin.x).add(tMax.y).add(c);
    buffer.setData(data.getData(), data.getSize());
    buffer.drawTriangles(6);
}

static void renderIcon(const Vector& min, const Vector& max, Ability a) {
    if (a == Ability::NONE) {
        return;
    }
    int id = a - 1;
    Vector tMin((id % 2) * 0.5f, (id / 2) * 0.5f);
    renderBox(min, max, tMin, tMin + Vector(0.5f, 0.5f),
              ColorUtils::setAlpha(AbilityUtils::getColor(a), 100));
}

void TextureRenderer::render(float lag) {
    (void)lag;
    shader.use();
    abilities.bindTo();
    renderIcon(Vector(0.5f, -0.5f), Vector(1.0f, -1.0f), Player::getAbility());
    renderIcon(Vector(0.75f, -0.25f), Vector(1.0f, -0.5f), Player::getPassiveAbility());
}