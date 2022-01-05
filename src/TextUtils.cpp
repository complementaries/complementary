#include "TextUtils.h"

#include <cmath>

#include "graphics/Font.h"
#include "graphics/Window.h"
#include "math/Matrix.h"
#include "objects/ObjectRenderer.h"
#include "player/Player.h"

void createTextBuffer(char* buffer, int bufSize, long ticks) {
    float seconds = Window::SECONDS_PER_TICK * ticks;
    float minutes = seconds / 60.f;
    snprintf(buffer, bufSize, "%02.0f:%05.2f", minutes, fmod(seconds, 60));
}

void TextUtils::drawTimer(Vector position, long ticks) {
    Matrix m;
    m.transform(Vector(-1.0f, 1.0f));
    m.scale(Vector(0.1f * Window::getHeight() / Window::getWidth(), -0.1f));

    char buffer[256];
    createTextBuffer(buffer, 256, ticks);

    Vector size(Font::getWidth(1.0f, buffer), 1.0f);
    Vector oversize = size * 0.1f;

    ObjectRenderer::addRectangle(position - oversize * 0.5f, size + oversize,
                                 Player::invertColors() ? ColorUtils::WHITE : ColorUtils::BLACK);
    ObjectRenderer::render(m);

    Font::prepare(m);
    Font::draw(position, 1.0f, Player::invertColors() ? ColorUtils::BLACK : ColorUtils::WHITE,
               buffer);
}

void TextUtils::drawPopupObjectSpace(Vector position, char* text, int alpha) {
    static constexpr float HEIGHT = 0.7f;
    float width = Font::getWidth(HEIGHT, text);
    Vector size(width, HEIGHT);
    Vector oversize = Vector(0.2f, 0.2f);
    position += Vector(width * -0.5f, 1 + oversize.y);

    Color col = Player::invertColors() ? ColorUtils::WHITE : ColorUtils::BLACK;
    col = ColorUtils::setAlpha(col, alpha);

    ObjectRenderer::addRectangle(position - oversize * 0.5f, size + oversize, col, -1.0f);
    ObjectRenderer::render();

    Font::prepare(-1.0f);
    col = Player::invertColors() ? ColorUtils::BLACK : ColorUtils::WHITE;
    col = ColorUtils::setAlpha(col, alpha);
    Font::draw(position, HEIGHT, col, text);
    Font::setZ(0.0f);
}

void TextUtils::drawBestTimeObjectSpace(Vector position, long ticks, int alpha) {
    char buffer[256];
    createTextBuffer(buffer, 256, ticks);

    float topWidth = Font::getWidth(1.f, "Best time");
    float bottomWidth = Font::getWidth(1.f, buffer);
    float width = std::max(topWidth, bottomWidth);
    Vector size(width, 2.0f);
    Vector oversize = size * 0.1f;
    position += Vector(width * -0.5f, Player::getSize().y + oversize.y);

    Color col = Player::invertColors() ? ColorUtils::WHITE : ColorUtils::BLACK;
    col = ColorUtils::setAlpha(col, alpha);

    ObjectRenderer::addRectangle(position - oversize * 0.5f, size + oversize, col, -1.0f);
    ObjectRenderer::render();

    Font::prepare(-1.0f);
    col = Player::invertColors() ? ColorUtils::BLACK : ColorUtils::WHITE;
    col = ColorUtils::setAlpha(col, alpha);
    Font::draw(position + Vector((width - topWidth) * 0.5f, 0.0f), 1.0f, col, "Best time");
    Font::draw(position + Vector((width - bottomWidth) * 0.5f, 1.0f), 1.0f, col, buffer);
    Font::setZ(0.0f);
}
