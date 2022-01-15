#include "TextUtils.h"

#include <cmath>

#include "Game.h"
#include "Menu.h"
#include "graphics/Font.h"
#include "graphics/RenderState.h"
#include "graphics/Window.h"
#include "math/Matrix.h"
#include "objects/ObjectRenderer.h"
#include "player/Player.h"
#include "tilemap/Tilemap.h"

constexpr float BEST_SIZE = 1.2f;
constexpr float TIME_SIZE = 1.2f;
constexpr float HELP_SIZE = 0.6f;

void createTextBuffer(char* buffer, int bufSize, int64_t ticks) {
    float seconds = Window::SECONDS_PER_TICK * ticks;
    int minutes = seconds / 60.f;
    snprintf(buffer, bufSize, "%02d:%05.2f", minutes, fmod(seconds, 60));
}

void createFullTextBuffer(char* buffer, int bufSize, int64_t ticks) {
    float seconds = Window::SECONDS_PER_TICK * ticks;
    int minutes = seconds / 60.f;
    snprintf(buffer, bufSize, "%02d:%05.2f", minutes, fmod(seconds, 60));
}

void TextUtils::drawTimer(long ticks) {
    Color light = Player::invertColors() ? ColorUtils::WHITE : ColorUtils::BLACK;
    Color dark = ColorUtils::invert(light);

    Matrix m;
    float factor = 0.1f * Window::getHeight() / Window::getWidth();
    m.transform(Vector(-1.0f, 1.0f));
    m.scale(Vector(factor, -0.1f));

    char buffer[256];
    createFullTextBuffer(buffer, 256, ticks);
    char deathBuffer[256];
    snprintf(deathBuffer, 256, "\6 %d", Player::getDeaths());
    char levelNumber[256];
    snprintf(levelNumber, 256, "# %d", Game::getCurrentLevel() + 1);

    Vector size(Font::getWidth(1.0f, buffer), 1.0f);
    Vector deathSize(Font::getWidth(1.0f, deathBuffer), 1.0f);
    Vector levelSize(Font::getWidth(1.0f, levelNumber), 1.0f);
    Vector oversize(0.3f, 0.1f);
    float right = 2.0f / factor;
    ObjectRenderer::addRectangle(Vector(), size + oversize, light);
    Vector startRight(right - deathSize.x - oversize.x, 0.0f);
    ObjectRenderer::addRectangle(startRight, deathSize + oversize, light);
    Vector startMid(right * 0.5 - (levelSize.x + oversize.x) * 0.5f, 0.0f);
    ObjectRenderer::addRectangle(startMid, levelSize + oversize, light);
    ObjectRenderer::render(m);

    Font::prepare(m);
    Font::draw(oversize * 0.5f, 1.0f, dark, buffer);
    Font::draw(startRight + oversize * 0.5f, 1.0f, dark, deathBuffer);
    Font::draw(startMid + oversize * 0.5f, 1.0f, dark, levelNumber);
}

void TextUtils::drawPopupObjectSpace(Vector position, char* text, int alpha) {
    static constexpr float HEIGHT = 0.7f;
    float width = Font::getWidth(HEIGHT, text);
    Vector size(width, HEIGHT);
    Vector oversize = Vector(0.2f, 0.2f);
    position += Vector(width * -0.5f, 1 + oversize.y);
    if (position.x < 0.0f) {
        position.x = 0.0f;
    }

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

void TextUtils::drawBestTimeObjectSpace(Vector position, int64_t ticks, int alpha) {
    char buffer[256];
    createTextBuffer(buffer, 256, ticks);

    float topWidth = Font::getWidth(BEST_SIZE, "Best time");
    float bottomWidth = Font::getWidth(TIME_SIZE, buffer);
    float height = BEST_SIZE + TIME_SIZE;
    char help[256] = {'\0'};
    float helpWidth = 0.0f;
    if (Game::canEnterLevel()) {
        height += HELP_SIZE;
        snprintf(help, 256, "Press [%s] to start", Menu::getJumpHelp());
        helpWidth = Font::getWidth(HELP_SIZE, help);
    }
    float width = std::max(topWidth, std::max(helpWidth, bottomWidth));
    Vector size(width, height);
    Vector oversize = Vector(0.5f, 0.25f);
    position += Vector(width * -0.5f, Player::getSize().y + oversize.y);

    Color col = Player::invertColors() ? ColorUtils::WHITE : ColorUtils::BLACK;
    col = ColorUtils::setAlpha(col, alpha);

    float mapWidth = RenderState::getXOffset() + Tilemap::getWidth() / 2;
    float mapStart = mapWidth - Tilemap::getWidth() / 2;
    Vector pos = position - oversize * 0.5f;
    Vector shift(0.0f, 0.0f);
    if (pos.x < mapStart) {
        shift.x = -pos.x + mapStart;
    }
    Vector end = pos + size + oversize;
    if (end.x > mapWidth) {
        shift = Vector(mapWidth - end.x, 0.0f);
    }
    ObjectRenderer::addRectangle(pos + shift, size + oversize, col, -1.0f);
    ObjectRenderer::render();

    Font::prepare(-1.0f);
    col = Player::invertColors() ? ColorUtils::BLACK : ColorUtils::WHITE;
    col = ColorUtils::setAlpha(col, alpha);
    Font::draw(shift + position + Vector((width - topWidth) * 0.5f, 0.0f), BEST_SIZE, col,
               "Best time");
    Font::draw(shift + position + Vector((width - bottomWidth) * 0.5f, BEST_SIZE), TIME_SIZE, col,
               buffer);
    if (Game::canEnterLevel()) {
        Font::draw(shift + position + Vector((width - helpWidth) * 0.5f, BEST_SIZE + TIME_SIZE),
                   HELP_SIZE, col, help);
    }
    Font::setZ(0.0f);
}

void TextUtils::drawStartHelp(Vector position, int alpha) {
    if (alpha <= 0) {
        return;
    }
    char help[256] = {'\0'};
    snprintf(help, 256, "Press [%s] to start", Menu::getJumpHelp());
    float width = Font::getWidth(1.0f, help);
    Vector size(width, 1.0f);
    Vector oversize = size * 0.1f;
    position += Vector(width * -0.5f, Player::getSize().y + oversize.y);

    Color col = Player::invertColors() ? ColorUtils::WHITE : ColorUtils::BLACK;
    col = ColorUtils::setAlpha(col, alpha);

    float mapWidth = RenderState::getXOffset() + Tilemap::getWidth() / 2;
    float mapStart = mapWidth - Tilemap::getWidth() / 2;
    Vector pos = position - oversize * 0.5f;
    Vector shift(0.0f, 0.0f);
    if (pos.x < mapStart) {
        shift.x = -pos.x + mapStart;
    }
    Vector end = pos + size + oversize;
    if (end.x > mapWidth) {
        shift = Vector(mapWidth - end.x, 0.0f);
    }

    ObjectRenderer::addRectangle(shift + position - oversize * 0.5f, size + oversize, col, -1.0f);
    ObjectRenderer::render();

    Font::prepare(-1.0f);
    col = Player::invertColors() ? ColorUtils::BLACK : ColorUtils::WHITE;
    col = ColorUtils::setAlpha(col, alpha);
    Font::draw(shift + position + Vector(0.0f, 0.0f), 1.0f, col, help);
    Font::setZ(0.0f);
}
