#include "Menu.h"

#include <string>
#include <vector>

#include "Input.h"
#include "graphics/Font.h"
#include "graphics/Window.h"
#include "objects/ObjectRenderer.h"
#include "player/Player.h"
#include "tilemap/Tilemap.h"

typedef void (*MenuFunction)();

struct MenuEntry {
    std::string text;
    float width;
    MenuFunction function;
};

std::vector<MenuEntry> lines;
static float fontSize = 4.0f;
static unsigned int menuIndex = 1;

static void nothing() {
}

static void quit() {
    Window::exit();
}

static void unpause() {
    Menu::clear();
}

static void restart() {
    unpause();
    Player::restart();
}

static void quitLevel() {
    Player::restart();
    Menu::showStartMenu();
}

static void add(const char* s, MenuFunction mf) {
    lines.push_back({s, 0.0f, mf});
}

static void openMenu() {
    if (Menu::isActive()) {
        return;
    }
    if (Input::getButton(ButtonType::PAUSE).pressedFirstFrame) {
        Menu::showPauseMenu();
    }
}

void Menu::tick() {
    openMenu();

    if (lines.size() == 0) {
        return;
    }
    if (Input::getButton(ButtonType::UP).pressedFirstFrame && menuIndex > 1) {
        menuIndex--;
    }
    if (Input::getButton(ButtonType::DOWN).pressedFirstFrame && menuIndex < lines.size() - 1) {
        menuIndex++;
    }
    if (Input::getButton(ButtonType::JUMP).pressedFirstFrame && menuIndex < lines.size()) {
        lines[menuIndex].function();
    }
}

void Menu::render(float lag) {
    (void)lag;

    Vector size;
    for (auto& e : lines) {
        e.width = Font::getWidth(fontSize, e.text.c_str());
        size.x = std::max(size.x, e.width);
        size.y += fontSize;
    }

    Vector overSize = size * 1.1f;
    Vector pos = (Tilemap::getSize() - overSize) * 0.5f;
    ObjectRenderer::prepare();
    ObjectRenderer::drawRectangle(pos, overSize, ColorUtils::setAlpha(ColorUtils::GRAY, 200));

    pos = (Tilemap::getSize() - Vector(0.0f, size.y)) * 0.5f;
    Font::prepare();
    unsigned int index = 0;
    for (auto& e : lines) {
        constexpr Color color[] = {ColorUtils::BLACK, ColorUtils::ORANGE};
        Font::draw(pos - Vector(e.width * 0.5f, 0.0f), fontSize, color[index == menuIndex],
                   e.text.c_str());
        pos.y += fontSize;
        index++;
    }
}

bool Menu::isActive() {
    return lines.size() > 0;
}

void Menu::clear() {
    lines.clear();
}

void Menu::showStartMenu() {
    clear();
    menuIndex = 1;
    add("[Complementary]", nothing);
    add("Start", unpause);
    add("Quit", quit);
}

void Menu::showPauseMenu() {
    clear();
    menuIndex = 1;
    add("[Pause]", nothing);
    add("Continue", unpause);
    add("Restart", restart);
    add("Quit Level", quitLevel);
}