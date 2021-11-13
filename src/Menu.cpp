#include "Menu.h"

#include <string>
#include <vector>

#include "Game.h"
#include "Input.h"
#include "graphics/Font.h"
#include "graphics/Window.h"
#include "objects/ObjectRenderer.h"
#include "player/Player.h"

typedef void (*MenuFunction)();

struct MenuEntry {
    std::string text;
    float width;
    MenuFunction function;
};

std::vector<MenuEntry> lines;
static float fontSize = 5.0f;
static unsigned int menuIndex = 1;
static MenuType type = MenuType::NONE;
constexpr float yGapFactor = 1.25f;
static bool showControls = false;

static void nothing() {
}

static void quit() {
    Window::exit();
}

static void unpause() {
    Menu::clear();
    type = MenuType::NONE;
}

static void start() {
    Game::exitTitleScreen();
    unpause();
}

static void restart() {
    unpause();
    Player::restart();
}

static void quitLevel() {
    unpause();
    Game::loadLevelSelect();
}

static void controls() {
    showControls = !showControls;
}

static void add(const char* s, MenuFunction mf) {
    lines.push_back({s, 0.0f, mf});
}

static void openMenu() {
    if (Menu::isActive() || Player::isDead()) {
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
        showControls = false;
    }
    if (Input::getButton(ButtonType::DOWN).pressedFirstFrame && menuIndex < lines.size() - 1) {
        menuIndex++;
        showControls = false;
    }
    if ((Input::getButton(ButtonType::JUMP).pressedFirstFrame ||
         Input::getButton(ButtonType::SWITCH).pressedFirstFrame) &&
        menuIndex < lines.size()) {
        Input::getButton(ButtonType::JUMP).reset();
        Input::getButton(ButtonType::SWITCH).reset();
        lines[menuIndex].function();
    }
}

static void renderControls(const Matrix& m, Vector pos, Vector baseSize) {
    // Links/Rechts gehen:
    // Jump: [A] / [B]
    // Fähigkeit: [X]
    // Weltwechsel: [L1] / [R1] bzw. [L] / [R]
    // Weltwechsel + Fähigkeit: [Y]

    constexpr const char* help[] = {
        "Left/Right:", "[LEFT JOYSTICK]",   "[LEFT D-PAD]", "[RIGHT D-PAD]", "Jump:",
        "[A] / [B]",   "Ability:",          "[X]",          "[L]",           "Switch:",
        "[R]",         "Switch + Ability:", "[Y]"};
    constexpr int length = sizeof(help) / sizeof(const char*);
    float smallFontSize = fontSize * 0.5f;
    Vector size;
    for (int i = 0; i < length; i++) {
        size.x = std::max(size.x, Font::getWidth(smallFontSize, help[i]));
        size.y += smallFontSize * yGapFactor;
    }
    Vector oversize = size * 1.1f;
    pos.y += baseSize.y * 0.5f - oversize.y * 0.5f;
    ObjectRenderer::addRectangle(pos, oversize, ColorUtils::setAlpha(ColorUtils::GRAY, 200));
    ObjectRenderer::render(m);
    pos += (oversize - size) * 0.5f;
    Font::prepare(m);
    constexpr Color colors[] = {ColorUtils::BLACK, ColorUtils::WHITE};
    for (int i = 0; i < length; i++) {
        Font::draw(pos, smallFontSize, colors[help[i][0] == '['], help[i]);
        pos.y += smallFontSize * yGapFactor;
    }
}

void Menu::render(float lag) {
    (void)lag;

    float aspect = static_cast<float>(Window::getWidth()) / Window::getHeight();
    Vector wSize(100.0f, 100.0f / aspect);
    Matrix m;
    m.transform(Vector(-1.0f, 1.0f));
    m.scale(Vector(2.0f, -2.0) / wSize);

    Vector size;
    for (auto& e : lines) {
        e.width = Font::getWidth(fontSize, e.text.c_str());
        size.x = std::max(size.x, e.width);
        size.y += fontSize * yGapFactor;
    }

    Vector overSize = size * 1.1f;
    Vector pos = (wSize - overSize) * 0.5f;
    if (type != MenuType::START) {
        ObjectRenderer::addRectangle(pos, overSize, ColorUtils::setAlpha(ColorUtils::GRAY, 200));
        ObjectRenderer::render(m);
    }

    pos = (wSize - Vector(0.0f, size.y)) * 0.5f;
    unsigned int index = 0;
    Font::prepare(m);
    for (auto& e : lines) {
        constexpr Color color[] = {ColorUtils::BLACK, ColorUtils::WHITE};
        if (index == menuIndex) {
            ObjectRenderer::addRectangle(pos - Vector(e.width * 0.5f + 0.3f, 0.f),
                                         Vector(e.width + 0.6f, fontSize), ColorUtils::BLACK);
            ObjectRenderer::render(m);
            Font::prepare(m);
        }
        Font::draw(pos - Vector(e.width * 0.5f, 0.0f), fontSize, color[index == menuIndex],
                   e.text.c_str());
        pos.y += fontSize * yGapFactor;
        index++;
    }
    if (showControls) {
        renderControls(m, (wSize - overSize) * 0.5f + Vector(overSize.x, 0.0f), overSize);
    }
}

bool Menu::isActive() {
    return type != MenuType::NONE;
}

void Menu::clear() {
    lines.clear();
}

void Menu::showStartMenu() {
    type = MenuType::START;
    clear();
    menuIndex = 1;
    add("[Complementary]", nothing);
    add("Start", start);
    add("Quit", quit);
}

void Menu::showPauseMenu() {
    type = MenuType::PAUSE;
    clear();
    menuIndex = 1;
    if (Game::getCurrentLevel() == -1) {
        add("[Pause]", nothing);
        add("Continue", unpause);
        add("Controls", controls);
        add("Quit", quit);
        return;
    }
    add("[Pause]", nothing);
    add("Continue", unpause);
    add("Restart", restart);
    add("Controls", controls);
    add("Quit Level", quitLevel);
}

MenuType Menu::getType() {
    return type;
}
