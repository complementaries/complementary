#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <imgui.h>
#include <string>
#include <vector>

#include "AbilityCutscene.h"
#include "Arguments.h"
#include "Clock.h"
#include "Game.h"
#include "GoalCutscene.h"
#include "Input.h"
#include "Menu.h"
#include "Savegame.h"
#include "Utils.h"
#include "graphics/Buffer.h"
#include "graphics/Font.h"
#include "graphics/RenderState.h"
#include "graphics/TextureRenderer.h"
#include "graphics/Window.h"
#include "graphics/gl/Glew.h"
#include "graphics/gl/Shader.h"
#include "graphics/gl/VertexBuffer.h"
#include "imgui/ImGuiUtils.h"
#include "objects/ColorObject.h"
#include "objects/DoorObject.h"
#include "objects/MovingObject.h"
#include "objects/ObjectRenderer.h"
#include "objects/Objects.h"
#include "objects/WindObject.h"
#include "particles/ParticleSystem.h"
#include "player/Player.h"
#include "sound/SoundManager.h"
#include "tilemap/Tilemap.h"
#include "tilemap/TilemapEditor.h"
#include "tilemap/Tiles.h"

static constexpr int MAX_LEVEL_NAME_LENGTH = 100;

static bool isInTitleScreen = true;
static int nextLevelIndex = -1;
static int currentLevelIndex = 0;
static Vector levelSelectPosition;
static std::vector<std::string> levelNames = {};
// TODO: make the level list configurable in the UI and get rid of this
static char currentLevelName[MAX_LEVEL_NAME_LENGTH] = "map0";

#ifndef NDEBUG
static char objectLoadLocation[MAX_LEVEL_NAME_LENGTH] = "assets/particlesystems/object.cmob";
static TilemapEditor* tilemapEditor;
#endif

static bool paused;
static bool singleStep;

static int fade = 0;
static int fadeAdd = 0;

static Clock tps;
static Clock fps;

static std::shared_ptr<ParticleSystem> titleEffectParticles;
static std::shared_ptr<ParticleSystem> backgroundParticles;
constexpr int BACKGROUND_PARTICLE_ALPHA_BLACK = 60;
constexpr int BACKGROUND_PARTICLE_ALPHA_WHITE = 40;

long totalTicks = 0;

static void loadTitleScreen();

static void findLevels() {
#ifdef _WIN32
    auto mapsDir = "assets\\maps";
#else
    auto mapsDir = "assets/maps";
#endif
    for (const auto& entry : std::filesystem::directory_iterator(mapsDir)) {
        auto pathString = entry.path().string();
#ifdef _WIN32
        auto isMap = pathString.rfind("assets\\maps\\map", 0) == 0;
#else
        auto isMap = pathString.rfind("assets/maps/map", 0) == 0;
#endif
        if (isMap && pathString.compare(pathString.size() - 5, 5, ".cmtm") == 0) {
            auto mapName = pathString.substr(12, pathString.size() - 12 -
                                                     5); // Cut off path and file extension
            levelNames.push_back(mapName);
        }
        std::sort(levelNames.begin(), levelNames.end());
    }
}

#ifndef NDEBUG
static void logGlError(const char* msg) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        fprintf(stderr, "GL-Error in %s: %u\n", msg, error);
    }
}
#endif

static void setBackgroundParticleColor() {
    auto particleColor = Player::invertColors() ? ColorUtils::WHITE : ColorUtils::BLACK;
    backgroundParticles->data.startColor = ColorUtils::setAlpha(particleColor, 0);
    backgroundParticles->data.endColor = ColorUtils::setAlpha(
        particleColor,
        Player::invertColors() ? BACKGROUND_PARTICLE_ALPHA_WHITE : BACKGROUND_PARTICLE_ALPHA_BLACK);
}

bool Game::init() {
    Tiles::init();
    if (Tilemap::init(48, 27) || Objects::init()) {
        return true;
    }
#ifndef NDEBUG
    if (TilemapEditor::init()) {
        return true;
    }
#endif
    if (RenderState::init() || ParticleRenderer::init() || Font::init() ||
        TextureRenderer::init() || Player::init() || Savegame::init() || AbilityCutscene::init() ||
        GoalCutscene::init()) {
        return true;
    }
    GoalTile::init();
    Savegame::load();

    findLevels();

    backgroundParticles =
        Objects::instantiateObject<ParticleSystem>("assets/particlesystems/background.cmob");
    backgroundParticles->destroyOnLevelLoad = false;
    setBackgroundParticleColor();
    backgroundParticles->play();
    for (int i = 0; i < 500; i++) {
        backgroundParticles->lateTick();
    }

    titleEffectParticles = Objects::instantiateObject<ParticleSystem>(
        "assets/particlesystems/titleeffect.cmob", Vector(24.f, 23.f));
    titleEffectParticles->destroyOnLevelLoad = false;

    loadTitleScreen();

#ifndef NDEBUG
    logGlError("init");
#endif

    return false;
}

static void onTileLoad() {
    for (int x = 0; x < Tilemap::getWidth(); x++) {
        for (int y = 0; y < Tilemap::getHeight(); y++) {
            Tilemap::getTile(x, y).onLoad(x, y);
        }
    }

    backgroundParticles->position = Tilemap::getSize() / 2;
}

static void loadLevel(const char* name) {
    Utils::print("Loading level %s\n", name);

    char formattedTilemapName[MAX_LEVEL_NAME_LENGTH];
    char formattedObjectmapName[MAX_LEVEL_NAME_LENGTH];

    if (snprintf(formattedTilemapName, MAX_LEVEL_NAME_LENGTH, "assets/maps/%s.cmtm", name) >
        MAX_LEVEL_NAME_LENGTH - 1) {
        puts("The level file name is too long!");
    }
    snprintf(formattedObjectmapName, MAX_LEVEL_NAME_LENGTH, "assets/maps/%s.cmom", name);
    snprintf(currentLevelName, MAX_LEVEL_NAME_LENGTH, "%s", name);

    Tilemap::load(formattedTilemapName);
    Objects::clear();
    Objects::load(formattedObjectmapName);
    onTileLoad();

    Game::setFade(254);
    Game::fadeIn(4);
    Player::restart();
}

static void loadTitleScreen() {
    loadLevel("title");
    isInTitleScreen = true;
    RenderState::setZoom(4.f, Vector(0.f, -2.f));
    Menu::showStartMenu();
    Player::setAbilities(Ability::WALL_JUMP, Ability::DASH, true);
    titleEffectParticles->play();
    SoundManager::playContinuousSound(Sound::TITLE);
}

void Game::exitTitleScreen() {
    isInTitleScreen = false;
    loadLevelSelect();
    titleEffectParticles->stop();
    titleEffectParticles->clear();
    SoundManager::playMusic();
    SoundManager::stopSound(Sound::TITLE);
    Player::setAbilities(Ability::NONE, Ability::NONE, false);
}

void Game::loadLevelSelect() {
    loadLevel("level_select");
    RenderState::setZoom(1.0f);
    if (levelSelectPosition.x != 0.0f || levelSelectPosition.y != 0.0f) {
        Player::setPosition(levelSelectPosition);
    }

    currentLevelIndex = -1;
    nextLevelIndex = -1;

    for (auto& obj : Objects::getObjects()) {
        // Destroy doors of accessible levels
        auto door = std::dynamic_pointer_cast<DoorObject>(obj);
        if (door != nullptr && door->data.type < Savegame::getCompletedLevels()) {
            door->destroy();
        }
    }
}

void Game::setNextLevelIndex(int next) {
    nextLevelIndex = next;
}

bool Game::inTitleScreen() {
    return isInTitleScreen;
}

void Game::nextLevel() {
    if (nextLevelIndex >= 0 && nextLevelIndex < static_cast<int>(levelNames.size())) {
        loadLevel(levelNames[nextLevelIndex].c_str());
        RenderState::setZoom(1.f);
        currentLevelIndex = nextLevelIndex;
        nextLevelIndex = -1;
    } else {
        Utils::print("Completed level with index %d\n", currentLevelIndex);
        if (currentLevelIndex >= Savegame::getCompletedLevels()) {
            Utils::print("Updated completion count to %d\n", currentLevelIndex + 1);
            Savegame::setCompletedLevels(currentLevelIndex + 1);
        }

        loadLevelSelect();
    }
}

void Game::switchWorld() {
    Player::toggleWorld();
    RenderState::addRandomizedShake(1.0f);
    RenderState::startMixing();
    RenderState::startGlowing();
    Input::playRumble(0.5f, 100);

    SoundManager::playSoundEffect(Sound::WORLD_SWITCH);
    SoundManager::switchMusic();

    setBackgroundParticleColor();
}

void Game::tick() {
    tps.update();
    if (paused) {
        if (singleStep) {
            singleStep = false;
        } else {
            return;
        }
    }
    fade = std::clamp(fade + fadeAdd, 0, 255);
    RenderState::tick();

    if (isInTitleScreen) {
        RenderState::setZoom(4.f, Vector(0.f, -2.f + sinf(totalTicks * 0.01f) * 0.13f));
    }

#ifndef NDEBUG
    if (tilemapEditor) {
        if (Input::getButton(ButtonType::SWITCH).pressedFirstFrame && Player::isAllowedToMove() &&
            !Player::isDead()) {
            switchWorld();
        }
        tilemapEditor->tick(Window::SECONDS_PER_TICK);
        if (Input::getButton(ButtonType::ABILITY).pressedFirstFrame) {
            tilemapEditor->flush();
            onTileLoad();
            delete tilemapEditor;
            tilemapEditor = nullptr;
        }

        float zoom = tilemapEditor->getZoom();
        if (Input::getButton(ButtonType::RIGHT).pressedFirstFrame) {
            zoom++;
        }
        if (Input::getButton(ButtonType::LEFT).pressedFirstFrame) {
            zoom--;
        }
        tilemapEditor->setZoom(zoom);
    } else
#endif
    {
        if (!ImGui::IsAnyItemActive()) {
            Menu::tick();
        }
        if (Menu::isActive() && Menu::getType() != MenuType::START) {
            return;
        }
        AbilityCutscene::tick();
        GoalCutscene::tick();
        Player::setAllowedToMove(!AbilityCutscene::isActive() && !GoalCutscene::isActive() &&
                                 !Menu::isActive() && !ImGui::IsAnyItemActive() &&
                                 !Player::isDead());
        if (Input::getButton(ButtonType::SWITCH).pressedFirstFrame && Player::isAllowedToMove()) {
            if (!Player::isCollidingWithAnyObject()) {
                switchWorld();
            }
        }
        Objects::tick();

        Player::tick();
        Objects::lateTick();
    }

    totalTicks++;
}

#ifndef NDEBUG
static void drawFpsDisplay() {
    Matrix m;
    m.transform(Vector(-1.0f, 1.0f));
    m.scale(Vector(0.1f * Window::getHeight() / Window::getWidth(), -0.1f));

    Font::prepare(m);
    char buffer[256];
    snprintf(buffer, 256, "FPS: %2.0f", fps.getUpdatesPerSecond());
    Font::draw(Vector(0.0f, 0.0f), 1.0f, ColorUtils::RED, buffer);
    snprintf(buffer, 256, "TPS: %3.0f", tps.getUpdatesPerSecond());
    Font::draw(Vector(0.0f, 1.0f), 1.0f, ColorUtils::RED, buffer);
    snprintf(buffer, 256, "Samples: %d", Arguments::samples);
    Font::draw(Vector(0.0f, 2.0f), 1.0f, ColorUtils::RED, buffer);
}
#endif

void Game::render(float lag) {
    if (Menu::isActive()) {
        lag = 0.0f;
    }
    fps.update();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    RenderState::bindAndClearDefaultFramebuffer();
#ifndef NDEBUG
    if (tilemapEditor) {
        RenderState::updateEditorViewMatrix(lag);
        tilemapEditor->render();
        return;
    }
#endif
    RenderState::updatePlayerViewMatrix(lag);
    RenderState::prepareEffectFramebuffer();
    Tilemap::renderBackground();
    Player::render(lag);

    RenderState::enableBlending();
    Tilemap::render();

    ParticleRenderer::prepare();
    Objects::render(lag);
    Objects::renderText(lag);
    ParticleRenderer::render();

    if (isInTitleScreen) {
        RenderState::updateViewMatrix(lag);

        glDisable(GL_DEPTH_TEST);
        Menu::render(lag);
        RenderState::renderTitleScreenEffects(lag);
    } else {
        RenderState::renderEffects(lag);
    }

    RenderState::disableBlending();
    RenderState::updateViewMatrix(lag);

    glDisable(GL_DEPTH_TEST);
    RenderState::enableBlending();
    if (!isInTitleScreen) {
        TextureRenderer::render(lag);
    }
    ObjectRenderer::prepare(Matrix());
    ObjectRenderer::drawRectangle(Vector(-1.0f, -1.0f), Vector(2.0f, 2.0f),
                                  ColorUtils::setAlpha(ColorUtils::BLACK, fade));
#ifndef NDEBUG
    drawFpsDisplay();
#endif
    if (!isInTitleScreen) {
        Menu::render(lag);
    }
    AbilityCutscene::render(lag);
    GoalCutscene::render(lag);
    RenderState::disableBlending();

#ifndef NDEBUG
    logGlError("render end");
#endif
}

#ifndef NDEBUG
void Game::renderImGui() {
    if (tilemapEditor) {
        return;
    }

    ImGui::Begin("DevGUI");

    if (ImGui::Button("Fade Out")) {
        fadeOut();
    }
    ImGui::SameLine();
    if (ImGui::Button("Fade In")) {
        fadeIn();
    }

    if (ImGui::Button("Next level")) {
        nextLevel();
    }
    ImGui::SameLine();
    if (ImGui::Button("Level select")) {
        loadLevelSelect();
    }

    ImGui::SameLine();
    if (ImGui::Button(paused ? "Unpause" : "Pause")) {
        paused = !paused;
    }

    if (paused && ImGui::Button("Next frame")) {
        singleStep = true;
    }

    if (ImGui::CollapsingHeader("Savegame", ImGuiTreeNodeFlags_DefaultOpen)) {
        int levels = Savegame::getCompletedLevels();
        ImGui::InputInt("Completed levels", &levels);
        if (levels != Savegame::getCompletedLevels()) {
            Savegame::setCompletedLevels(levels);
        }

        if (ImGui::Button("Reset save")) {
            Savegame::reset();
        }
    }

    if (ImGui::CollapsingHeader("Tilemap", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Width: %d, Height: %d", Tilemap::getWidth(), Tilemap::getHeight());

        if (!tilemapEditor && ImGui::Button("Open Editor")) {
            tilemapEditor = new TilemapEditor(Window::getWidth(), Window::getHeight());
        }

        int zoom = static_cast<int>(tilemapEditor->getZoom());
        ImGui::SameLine();
        ImGui::SliderInt("Zoom", &zoom, 1, 3);
        tilemapEditor->setZoom(zoom);

        ImGui::InputText("Level name", currentLevelName, 50);
        static char tileMapName[MAX_LEVEL_NAME_LENGTH];
        snprintf(tileMapName, MAX_LEVEL_NAME_LENGTH, "assets/maps/%s.cmtm", currentLevelName);
        static char objectMapName[MAX_LEVEL_NAME_LENGTH];
        snprintf(objectMapName, MAX_LEVEL_NAME_LENGTH, "assets/maps/%s.cmom", currentLevelName);

        if (ImGui::Button("Load")) {
            loadLevel(currentLevelName);
        }

        ImGui::SameLine();
        if (ImGui::Button("Save")) {
            Tilemap::save(tileMapName);
            Objects::save(objectMapName);
        }
    }

    if (ImGui::CollapsingHeader("Prototypes")) {
        if (ImGui::Button("Load from file")) {
            auto obj = Objects::loadObject(objectLoadLocation);
            Objects::addPrototype(obj);
        }
        ImGui::SameLine();
        ImGui::InputText("##loc", objectLoadLocation, MAX_LEVEL_NAME_LENGTH);

        for (size_t i = 0; i < Objects::getPrototypeCount(); i++) {
            auto prototype = Objects::getPrototype(i);

            char header[128];
            snprintf(header, 128, "#%zu: %s", i, prototype->getTypeName());

            ImGui::Indent();
            ImGui::PushID(header);
            if (ImGui::CollapsingHeader(header)) {
                ImGui::PushDisabled();
                prototype->renderImGui();
                ImGui::PopDisabled();

                if (ImGui::Button("Spawn")) {
                    Objects::instantiateObject(i);
                }

                ImGui::SameLine();
                if (ImGui::Button("Spawn at player position")) {
                    auto obj = Objects::instantiateObject(i);
                    obj->position = Player::getPosition();
                }
            }
            ImGui::PopID();
            ImGui::Unindent();
        }
    }

    if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen)) {
        Player::renderImGui();
    }

    if (ImGui::CollapsingHeader("Input Debug")) {
        ImGui::PushDisabled();
        for (int i = 0; i < (int)ButtonType::MAX; i++) {
            auto button = Input::getButton((ButtonType)i);
            ImGui::Checkbox(Input::getButtonName((ButtonType)i), &button.pressed);
            ImGui::SameLine();
            ImGui::Checkbox("FirstFrame", &button.pressedFirstFrame);
            ImGui::SameLine();
            ImGui::Text("PressedTicks: %d", button.pressedTicks);
        }

        float horizontal = Input::getHorizontal();
        ImGui::SliderFloat("Horizontal", &horizontal, -1, 1);
        ImGui::PopDisabled();
    }

    for (size_t i = 0; i < Objects::getObjects().size(); i++) {
        auto object = Objects::getObjects()[i];

        const char* destructionInfo = "";
        if (!object->destroyOnLevelLoad) {
            destructionInfo = "[PERSISTENT] ";
        }
        if (object->shouldDestroy) {
            destructionInfo = "[QUEUED FOR DESTRUCTION] ";
        }

        // Remove common prefixes
        const char* displayFilePath = object->filePath;
        if (strncmp(displayFilePath, "assets/", 7) == 0) {
            displayFilePath += 7;
        }

        char header[256];
        snprintf(header, 256, "%s#%zu: %s [%s] (prototype #%d)###%zu", destructionInfo, i,
                 object->getTypeName(), displayFilePath, object->prototypeId, i);
        char id[128];
        snprintf(id, 128, "%zu", i);

        ImGui::PushID(header);
        if (ImGui::CollapsingHeader(header)) {
            ImGui::DragFloat2("Position", object->position.data());
            ImGui::InputInt("Prototype ID (dangerous)", &object->prototypeId);
            ImGui::Spacing();

            object->renderImGui();
            if (ImGui::Button("Destroy")) {
                object->destroy();
            }

            int pathStrLen = strlen(object->filePath);
            bool isValidExtension = strncmp(object->filePath + (pathStrLen - 5), ".cmob", 5) == 0;
            if (!isValidExtension) {
                ImGui::PushDisabled();
            }

            if (ImGui::Button("Save")) {
                Objects::saveObject(object->filePath, *object);
            }

            if (!isValidExtension) {
                ImGui::PopDisabled();
            }

            ImGui::SameLine();
            ImGui::InputText("##loc", object->filePath, MAX_LEVEL_NAME_LENGTH);
        }
        ImGui::PopID();
    }

    ImGui::End();
}
#endif

void Game::onWindowResize(int width, int height) {
    glViewport(0, 0, width, height);
#ifndef NDEBUG
    if (tilemapEditor) {
        tilemapEditor->onScreenResize(width, height);
    }
#endif
    RenderState::resize(width, height);
}

void Game::onMouseEvent(void* eventPointer) {
#ifndef NDEBUG
    if (tilemapEditor) {
        tilemapEditor->onMouseEvent(eventPointer);
    }
#endif
}

void Game::fadeIn(int speed) {
    fadeAdd = -speed;
}

void Game::fadeOut(int speed) {
    fadeAdd = speed;
}

void Game::setFade(int amount) {
    fade = amount;
    fadeAdd = 0;
}

bool Game::isFading() {
    return fade != 0 && fade != 255;
}

int Game::getCurrentLevel() {
    return currentLevelIndex;
}

void Game::setLevelScreenPosition(const Vector& v) {
    levelSelectPosition = v;
}