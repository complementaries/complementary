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
#include "Game.h"
#include "GoalCutscene.h"
#include "Input.h"
#include "Menu.h"
#include "Profiler.h"
#include "Savegame.h"
#include "TextUtils.h"
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
#include "objects/LevelDoorObject.h"
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
static GameMode mode = GameMode::DEFAULT;
static bool showingSpeedrunResult = false;

#ifndef NDEBUG
static char objectLoadLocation[MAX_LEVEL_NAME_LENGTH] = "assets/particlesystems/object.cmob";
static TilemapEditor* tilemapEditor;
#endif

static bool paused;
static bool singleStep;
static bool levelAvailable = false;
static int levelAvailableAlpha = 0;

static int fade = 0;
static int fadeAdd = 0;

static std::shared_ptr<ParticleSystem> titleEffectParticles;
static std::shared_ptr<ParticleSystem> backgroundParticles;
constexpr int BACKGROUND_PARTICLE_ALPHA_BLACK = 60;
constexpr int BACKGROUND_PARTICLE_ALPHA_WHITE = 40;

constexpr int MAX_WORLD_SWITCH_BUFFER = 9;
static int worldSwitchBuffer = 0;

long totalTicks = 0;
long timerTicks = 0;

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

void Game::setBackgroundParticleColor() {
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

static bool loadLevel(const char* name) {
#ifndef NDEBUG
    if (strcmp(name, "_autosave") != 0) {
        Utils::print("Creating autosave.\n", name);
        Objects::save("assets/maps/_autosave.cmom");
        Tilemap::save("assets/maps/_autosave.cmtm");
    }
#endif
    Utils::print("Loading level %s\n", name);

    char formattedTilemapName[MAX_LEVEL_NAME_LENGTH];
    char formattedObjectmapName[MAX_LEVEL_NAME_LENGTH];

    if (snprintf(formattedTilemapName, MAX_LEVEL_NAME_LENGTH, "assets/maps/%s.cmtm", name) >
        MAX_LEVEL_NAME_LENGTH - 1) {
        puts("The level file name is too long!");
    }
    snprintf(formattedObjectmapName, MAX_LEVEL_NAME_LENGTH, "assets/maps/%s.cmom", name);
    snprintf(currentLevelName, MAX_LEVEL_NAME_LENGTH, "%s", name);

    if (Tilemap::load(formattedTilemapName)) {
        return true;
    }
    Objects::clear();
    if (Objects::load(formattedObjectmapName)) {
        return true;
    }
    onTileLoad();

    Game::setFade(254);
    Game::fadeIn(4);
    Player::restart();
    Game::setBackgroundParticleColor();

    Objects::tick();
    Objects::lateTick();
    return false;
}

void Game::loadTitleScreen() {
    bool titleLoaded = loadLevel("title");
    (void)titleLoaded;
    assert(!titleLoaded);
    isInTitleScreen = true;
    RenderState::setZoom(4.f, Vector(0.f, -2.f));
    Menu::showStartMenu();
    Player::setAbilities(Ability::WALL_JUMP, Ability::DASH, true);
    titleEffectParticles->play();
    SoundManager::stopSound(Sound::LIGHT_BG);
    SoundManager::stopSound(Sound::DARK_BG);
    SoundManager::playContinuousSound(Sound::TITLE);
    currentLevelIndex = 0;
}

void Game::exitTitleScreen(GameMode gameMode) {
    mode = gameMode;
    if (mode == GameMode::DEFAULT) {
        if (loadLevelSelect()) {
            return;
        }
    } else if (mode == GameMode::SPEEDRUN) {
        nextLevelIndex = 0;
        nextLevel();
    }
    isInTitleScreen = false;
    titleEffectParticles->stop();
    titleEffectParticles->clear();
    SoundManager::playMusic();
    SoundManager::stopSound(Sound::TITLE);
    Player::setAbilities(Ability::NONE, Ability::NONE, false);
    timerTicks = 0;
}

bool Game::loadLevelSelect() {
    if (loadLevel("level_select")) {
        return true;
    }
    RenderState::setZoom(1.0f);
    if (levelSelectPosition.x != 0.0f || levelSelectPosition.y != 0.0f) {
        Player::setPosition(levelSelectPosition);
    }

    currentLevelIndex = -1;
    nextLevelIndex = -1;

    for (auto& obj : Objects::getObjects()) {
        // Destroy doors of accessible levels
        auto door = std::dynamic_pointer_cast<LevelDoorObject>(obj);
        if (door != nullptr && door->data.type < Savegame::getCompletedLevels()) {
            door->open();
        }
    }
    return false;
}

void Game::setNextLevelIndex(int next) {
    nextLevelIndex = next;
}

bool Game::inTitleScreen() {
    return isInTitleScreen;
}

void Game::nextLevel() {
    worldSwitchBuffer = 0;
    if (nextLevelIndex >= 0 && nextLevelIndex < static_cast<int>(levelNames.size())) {
        if (loadLevel(levelNames[nextLevelIndex].c_str())) {
            return;
        }
        RenderState::setZoom(1.f);
        currentLevelIndex = nextLevelIndex;
        if (mode == GameMode::SPEEDRUN) {
            nextLevelIndex = currentLevelIndex + 1;
        } else {
            nextLevelIndex = -1;
        }
    } else if (mode == GameMode::DEFAULT) {
        Utils::print("Completed level with index %d\n", currentLevelIndex);

        bool shouldSave = false;
        uint32_t bestCompletionTime = Savegame::getCompletionTime(currentLevelIndex);
        Utils::print("Completed level %d in %d ticks. Best time: %d \n", currentLevelIndex,
                     timerTicks, bestCompletionTime);
        if (timerTicks < bestCompletionTime || bestCompletionTime == 0) {
            Savegame::setCompletionTime(currentLevelIndex, timerTicks);
            Utils::print("Saving record time\n");
            shouldSave = true;
        }
        if (currentLevelIndex >= Savegame::getCompletedLevels()) {
            Savegame::setCompletedLevels(currentLevelIndex + 1);
            Utils::print("Updated completion count to %d.\n", currentLevelIndex + 1);
            shouldSave = true;
        }
        if (shouldSave) {
            Savegame::save();
        }

        loadLevelSelect();
    } else if (mode == GameMode::SPEEDRUN) {
        showingSpeedrunResult = true;
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
    ObjectRenderer::clearStaticBuffer();
}

static void playFakeSwitchAnimation() {
    RenderState::addRandomizedShake(1.0f);
    RenderState::startFakeMixing();
    RenderState::startGlowing();
    Input::playRumble(0.5f, 100);

    SoundManager::playSoundEffect(Sound::WORLD_SWITCH);
}

static void tickLevelSelect() {
    levelAvailable = false;
    if (Game::getCurrentLevel() == -1 && !GoalCutscene::isActive()) {
        Vector center = Player::getCenter();
        int minX = std::max(static_cast<int>(center.x - 1.5f), 0);
        int maxX = std::min(static_cast<int>(center.x + 2.5f), Tilemap::getWidth());
        int y = center.y;
        constexpr float startDistance = 5.0f;
        float minDistance = startDistance;
        Vector goal;
        Face face = Face::LEFT;
        for (int x = minX; x < maxX; x++) {
            const Tile& tile = Tilemap::getTile(x, y);
            if (tile == Tiles::GOAL_LEFT || tile == Tiles::GOAL_RIGHT) {
                float distance = std::abs(x - center.x);
                if (distance < minDistance) {
                    face = tile == Tiles::GOAL_LEFT ? Face::LEFT : Face::RIGHT;
                    minDistance = distance;
                    goal = Vector(x, y);
                }
            }
        }
        if (minDistance < startDistance) {
            if (Input::getButton(ButtonType::JUMP).pressedFirstFrame) {
                GoalCutscene::show(goal, face);
                Game::setLevelScreenPosition(goal + FaceUtils::getDirection(face) * 2.0f);
            }
            levelAvailable = true;
        }
    }
    levelAvailableAlpha += (levelAvailable * 2 - 1) * 10;
    levelAvailableAlpha = std::clamp(levelAvailableAlpha, 0, 255);
}

void Game::tick() {
#ifndef NDEBUG
    Profiler::tick();
    Profiler::Timer tickTimer(Profiler::tickNanos);
#endif
    if (paused) {
        if (singleStep) {
            singleStep = false;
        } else {
            return;
        }
    }
    fade = std::clamp(fade + fadeAdd, 0, 255);
    RenderState::tick();
    tickLevelSelect();

    if (showingSpeedrunResult) {
        if (Input::getButton(ButtonType::JUMP).pressedFirstFrame) {
            showingSpeedrunResult = false;
            loadTitleScreen();
        }
        return;
    }

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
        if (Input::getButton(ButtonType::ABILITY).pressedFirstFrame ||
            Input::getButton(ButtonType::SWITCH_AND_ABILITY).pressedFirstFrame) {
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
        if (Player::isDead()) {
            worldSwitchBuffer = 0;
        }
        Player::setAllowedToMove(!AbilityCutscene::isActive() && !GoalCutscene::isActive() &&
                                 !Menu::isActive() && !ImGui::IsAnyItemActive() &&
                                 !Player::isDead());
        if (Input::getButton(ButtonType::SWITCH).pressedFirstFrame ||
            Input::getButton(ButtonType::SWITCH_AND_ABILITY).pressedFirstFrame) {
            worldSwitchBuffer = MAX_WORLD_SWITCH_BUFFER;
            playFakeSwitchAnimation();
        }
        if (Player::isAllowedToMove()) {
            worldSwitchBuffer -= worldSwitchBuffer > 0;
            if (!Player::isCollidingInAnyWorld() && worldSwitchBuffer > 0) {
                worldSwitchBuffer = 0;
                switchWorld();
            } else if (worldSwitchBuffer == 1) {
                worldSwitchBuffer = 0;
            }
        }
#ifndef NDEBUG
        {
            Profiler::Timer timer(Profiler::objectTickNanos);
            Objects::tick();
        }
        {
            Profiler::Timer timer(Profiler::playerTickNanos);
            Player::tick();
        }
        {
            Profiler::Timer timer(Profiler::objectLateTickNanos);
            Objects::lateTick();
        }
#else
        Objects::tick();
        Player::tick();
        Objects::lateTick();
#endif
    }

    totalTicks++;
    if (Player::isAllowedToMove()) {
        timerTicks++;
        if (timerTicks > UINT32_MAX) {
            // Sanitize high counter values
            timerTicks = UINT32_MAX;
        }
    }
}

bool Game::canEnterLevel() {
    return levelAvailable;
}

int Game::levelStartAlpha() {
    return levelAvailableAlpha;
}

#ifndef NDEBUG
static void drawFpsDisplay() {
    Matrix m;
    m.transform(Vector(-1.0f, 1.0f));
    m.scale(Vector(0.1f * Window::getHeight() / Window::getWidth(), -0.1f));

    Font::prepare(m);
    char buffer[256];
    snprintf(buffer, 256, "FPS: %2.0f", Profiler::getFPS());
    Font::draw(Vector(00.0f, 2.0f), 0.6f, ColorUtils::RED, buffer);
    snprintf(buffer, 256, "TPS: %3.0f", Profiler::getTPS());
    Font::draw(Vector(00.0f, 2.6f), 0.6f, ColorUtils::RED, buffer);
    snprintf(buffer, 256, "Samples: %d", Arguments::samples);
    Font::draw(Vector(00.0f, 3.2f), 0.6f, ColorUtils::RED, buffer);
}
#endif

void Game::render(float lag) {
#ifndef NDEBUG
    Profiler::render();
    Profiler::Timer renderTimer(Profiler::renderNanos);
#endif
    if (Menu::isActive()) {
        lag = 0.0f;
    }
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    RenderState::bindAndClearDefaultFramebuffer();
#ifndef NDEBUG
    if (tilemapEditor) {
        tilemapEditor->render();
        return;
    }
#endif
    RenderState::updatePlayerViewMatrix(lag);
    RenderState::prepareEffectFramebuffer();

    Tilemap::renderBackground();
    if (!showingSpeedrunResult) {
        Player::render(lag);

        RenderState::enableBlending();
        Tilemap::render();

        ParticleRenderer::prepare();
#ifndef NDEBUG
        {
            Profiler::Timer timer(Profiler::objectRenderNanos);
            Objects::render(lag);
        }
        {
            Profiler::Timer timer(Profiler::objectTextRenderNanos);
            Objects::renderText(lag);
        }
        {
            Profiler::Timer timer(Profiler::particleRenderNanos);
            ParticleRenderer::render();
        }
#else
        Objects::render(lag);
        Objects::renderText(lag);
        ParticleRenderer::render();
#endif
        Tilemap::renderForeground();
    }

    glDisable(GL_DEPTH_TEST);

    if (getCurrentLevel() == -1) {
        Color c = Player::invertColors() ? ColorUtils::WHITE : ColorUtils::BLACK;
        float x = RenderState::getXOffset();
        float offset = -x + Tilemap::getWidth() * 0.5f;
        ObjectRenderer::addRectangle(Vector(offset - 0.02f, 0.0f),
                                     Vector(Tilemap::getWidth() / 2, Tilemap::getHeight()),
                                     ColorUtils::setAlpha(c, 200));
        ObjectRenderer::render();
    }

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
    if (!isInTitleScreen && currentLevelIndex > -1) {
        TextUtils::drawTimer(Vector(0.2f, 0.2f), timerTicks);
    }
    ObjectRenderer::addRectangle(Vector(-1.0f, -1.0f), Vector(2.0f, 2.0f),
                                 ColorUtils::setAlpha(ColorUtils::BLACK, fade));
    ObjectRenderer::render(Matrix());

#ifndef NDEBUG
    drawFpsDisplay();
#endif

    Font::prepare();

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

    if (!ImGui::Begin("DevGUI")) {
        ImGui::End();
        return;
    }

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
            Savegame::save();
        }

        if (ImGui::Button("Reset save")) {
            Savegame::reset();
            Savegame::save();
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
            if (loadLevel(currentLevelName)) {
                Utils::print("Failed to load map.\n");
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Save")) {
            if (Tilemap::save(tileMapName) || Objects::save(objectMapName)) {
                Utils::print("Failed to save map.\n");
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Load autosave")) {
            if (loadLevel("_autosave")) {
                Utils::print("Failed to load map.\n");
            }
        }
    }

    if (ImGui::CollapsingHeader("Levels")) {
        for (auto& levelName : levelNames) {
            if (ImGui::Button(levelName.c_str())) {
                if (loadLevel(levelName.c_str())) {
                    Utils::print("Failed to load map.\n");
                }
            }
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

    if (ImGui::CollapsingHeader("Player")) {
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

    if (ImGui::CollapsingHeader("Objects")) {
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
                bool isValidExtension =
                    strncmp(object->filePath + (pathStrLen - 5), ".cmob", 5) == 0;
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

GameMode Game::getMode() {
    return mode;
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
    return fade > 200;
}

int Game::getCurrentLevel() {
    return currentLevelIndex;
}

void Game::setLevelScreenPosition(const Vector& v) {
    levelSelectPosition = v;
}

void Game::resetTickCounter() {
    if (mode != GameMode::SPEEDRUN) {
        timerTicks = 0;
    }
}

void Game::pause() {
    paused = true;
}

bool Game::isPaused() {
    return paused;
}
