#include "Menu.h"

#include <string>
#include <vector>

#include "Game.h"
#include "Input.h"
#include "graphics/Font.h"
#include "graphics/Window.h"
#include "objects/ObjectRenderer.h"
#include "player/Player.h"

typedef std::vector<const char*> Help;

const char* ABILITY = "Ability:";
const char* SWITCH = "Switch:";
const char* ABILITY_SWITCH = "Switch + Ability:";

Help HELP_KEYBOARD = {"Left/Right:", "[A",           "[D",          "[LEFT-ARROW", "[RIGHT-ARROW",
                      "Jump:",       "[SPACE",       ABILITY,       "[LEFT-SHIFT", SWITCH,
                      "[ENTER",      ABILITY_SWITCH, "[RIGHT-SHIFT"};
Help HELP_XBOX = {"Left/Right:", "[\5",      "Jump:", "[A",       "[B",           ABILITY,
                  "[X",          "[L1 / L2", SWITCH,  "[R1 / R2", ABILITY_SWITCH, "[Y"};
Help HELP_PS = {"Left/Right:", "[\5",      "Jump:", "[\1",      "[\4",          ABILITY,
                "[\3",         "[L1 / L2", SWITCH,  "[R1 / R2", ABILITY_SWITCH, "[\2"};
Help HELP = {"Left/Right:", "[\5",     "Jump:", "[A",      "[B",           ABILITY,
             "[Y",          "[L / ZL", SWITCH,  "[R / ZR", ABILITY_SWITCH, "[X"};

static const Help& getRelevantHelp() {
    SDL_GameController* c = Input::getController();
    if (c == nullptr) {
        return HELP_KEYBOARD;
    }
    SDL_GameControllerType type = SDL_GameControllerGetType(c);
    if (type == SDL_CONTROLLER_TYPE_XBOX360 || type == SDL_CONTROLLER_TYPE_XBOXONE) {
        return HELP_XBOX;
    } else if (type == SDL_CONTROLLER_TYPE_PS3 || type == SDL_CONTROLLER_TYPE_PS4) {
        return HELP_PS;
    }
    return HELP;
}

static int searchFor(const Help& help, const char* search) {
    for (unsigned int i = 0; i < help.size(); i++) {
        if (help[i] == search) {
            return i + 1;
        }
    }
    return -1;
}

static const char* getHelp(const char* search) {
    const Help& help = getRelevantHelp();
    int index = searchFor(help, search);
    return index < 0 ? "ERROR" : help[index] + 1;
}

const char* Menu::getSwitchHelp() {
    return getHelp(SWITCH);
}

const char* Menu::getAbilityHelp() {
    return getHelp(ABILITY);
}

const char* Menu::getAbilitySwitchHelp() {
    return getHelp(ABILITY_SWITCH);
}

typedef void (*MenuFunction)();

struct MenuEntry {
    std::string text;
    float width;
    MenuFunction function;
};

std::vector<MenuEntry> lines;
static float fontSize = 4.5f;
static unsigned int menuIndex = 1;
static MenuType type = MenuType::NONE;
constexpr float yGapFactor = 1.25f;
static bool showControls = false;
static bool closeWithPause = false;

static void nothing() {
}

static void quit() {
    Window::exit();
}

static void unpause() {
    Menu::clear();
    closeWithPause = false;
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

static void toggleFullscreen() {
    Window::toggleFullscreen();
    Menu::showPauseMenu();
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
    if (Input::getButton(ButtonType::PAUSE).pressedFirstFrame && closeWithPause) {
        unpause();
        return;
    }

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

static void renderControls(const Matrix& m, Vector pos, Vector baseSize, const Help& help) {
    float smallFontSize = fontSize * 0.5f;
    Vector size;
    for (unsigned int i = 0; i < help.size(); i++) {
        size.x = std::max(size.x, Font::getWidth(smallFontSize, help[i]));
        size.y += smallFontSize * yGapFactor;
    }
    Vector oversize = size * 1.1f;
    pos.y += baseSize.y * 0.5f - oversize.y * 0.5f;
    ObjectRenderer::addRectangle(pos, oversize, ColorUtils::setAlpha(ColorUtils::GRAY, 200));
    ObjectRenderer::render(m);
    pos += (oversize - size) * 0.5f;
    Font::prepare(m);
    for (unsigned int i = 0; i < help.size(); i++) {
        if (help[i][0] == '[') {
            Vector shifted = pos;
            Font::draw(shifted, smallFontSize, ColorUtils::WHITE, "[");
            shifted.x += Font::getWidth(smallFontSize, "[");
            Font::draw(shifted, smallFontSize, ColorUtils::ORANGE, help[i] + 1);
            shifted.x += Font::getWidth(smallFontSize, help[i] + 1);
            Font::draw(shifted, smallFontSize, ColorUtils::WHITE, "]");
        } else {
            Font::draw(pos, smallFontSize, ColorUtils::BLACK, help[i]);
        }
        pos.y += smallFontSize * yGapFactor;
    }
}

static void renderControls(const Matrix& m, Vector pos, Vector baseSize) {
    renderControls(m, pos, baseSize, getRelevantHelp());
}

void Menu::render(float lag) {
    if (type == MenuType::NONE) {
        return;
    }
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
        ObjectRenderer::addRectangle(Vector(0, 0), Vector(Window::getWidth(), Window::getHeight()),
                                     ColorUtils::setAlpha(ColorUtils::BLACK, 160));
        ObjectRenderer::addRectangle(pos, overSize, ColorUtils::setAlpha(ColorUtils::WHITE, 255));
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
    closeWithPause = false;
}

static void fullScreenOption() {
    if (Window::isFullscreen()) {
        add("Set Windowed", toggleFullscreen);
    } else {
        add("Set Fullscreen", toggleFullscreen);
    }
}

void Menu::showPauseMenu() {
    type = MenuType::PAUSE;
    clear();
    menuIndex = 1;
    closeWithPause = true;
    if (Game::getCurrentLevel() == -1) {
        add("[Pause]", nothing);
        add("Continue", unpause);
        add("Controls", controls);
        fullScreenOption();
        add("Quit", quit);
        return;
    }
    add("[Pause]", nothing);
    add("Continue", unpause);
    add("Restart", restart);
    add("Controls", controls);
    fullScreenOption();
    add("Quit Level", quitLevel);
}

MenuType Menu::getType() {
    return type;
}
