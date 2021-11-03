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
    ObjectRenderer::prepare(m);
    ObjectRenderer::drawRectangle(position, size,
                                  Player::invertColors() ? ColorUtils::WHITE : ColorUtils::BLACK);

    Font::prepare(m);
    Font::draw(position, 1.0f, Player::invertColors() ? ColorUtils::BLACK : ColorUtils::WHITE,
               buffer);
}

void TextUtils::drawBestTimeObjectSpace(Vector position, long ticks, int alpha) {
    char buffer[256];
    createTextBuffer(buffer, 256, ticks);

    float topWidth = Font::getWidth(1.f, "Best time");
    float bottomWidth = Font::getWidth(1.f, buffer);
    float width = std::max(topWidth, bottomWidth);
    Vector size(width, 2.0f);
    ObjectRenderer::prepare();
    ObjectRenderer::setZ(0.0f);

    Color col = Player::invertColors() ? ColorUtils::WHITE : ColorUtils::BLACK;
    col = ColorUtils::setAlpha(col, alpha);
    ObjectRenderer::drawRectangle(position - size / 2, size, col);

    Font::prepare();
    col = Player::invertColors() ? ColorUtils::BLACK : ColorUtils::WHITE;
    col = ColorUtils::setAlpha(col, alpha);
    Font::draw(position - size / 2, 1.0f, col, "Best time");
    Font::draw(position - size / 2 + Vector(0.f, 1.f), 1.0f, col, buffer);
}
