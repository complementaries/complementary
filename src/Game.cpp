#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <imgui.h>
#include <string>
#include <vector>

#include "Game.h"
#include "Input.h"
#include "graphics/Buffer.h"
#include "graphics/RenderState.h"
#include "graphics/Window.h"
#include "graphics/gl/Glew.h"
#include "graphics/gl/Shader.h"
#include "graphics/gl/VertexBuffer.h"
#include "imgui/ImGuiUtils.h"
#include "math/Matrix.h"
#include "objects/ColorObject.h"
#include "objects/MovingObject.h"
#include "objects/ObjectRenderer.h"
#include "objects/Objects.h"
#include "objects/WindObject.h"
#include "particles/ParticleSystem.h"
#include "player/Player.h"
#include "tilemap/Tilemap.h"
#include "tilemap/TilemapEditor.h"
#include "tilemap/Tiles.h"

static constexpr int MAX_LEVEL_NAME_LENGTH = 100;

static std::vector<const char*> levelNames = {"map0", "map1"};
static int levelIndex = 0;
// TODO: make the level list configurable in the UI and get rid of this
static char currentLevelName[MAX_LEVEL_NAME_LENGTH] = "assets/maps/map0";
static char objectSaveLocation[MAX_LEVEL_NAME_LENGTH] = "assets/subdir/object.cmob";
static char objectLoadLocation[MAX_LEVEL_NAME_LENGTH] = "assets/subdir/object.cmob";

static TilemapEditor* tilemapEditor;
static bool paused;
static bool singleStep;

static std::shared_ptr<ParticleSystem> testParticleSystem;

bool Game::init() {
    Tiles::init();
    if (Tilemap::init(48, 27) || Player::init() || Objects::init() || TilemapEditor::init() ||
        RenderState::init() || ParticleRenderer::init()) {
        return true;
    }

    testParticleSystem = Objects::loadObject<ParticleSystem>("assets/particlesystems/test.cmob");

    nextLevel();
    return false;
}

void Game::nextLevel() {
    const char* level = levelNames[levelIndex];

    printf("Loading level %s\n", level);

    char formattedTilemapName[MAX_LEVEL_NAME_LENGTH];
    char formattedObjectmapName[MAX_LEVEL_NAME_LENGTH];

    if (snprintf(formattedTilemapName, MAX_LEVEL_NAME_LENGTH, "assets/maps/%s.cmtm", level) >
        MAX_LEVEL_NAME_LENGTH - 1) {
        puts("The level file name is too long!");
    }
    snprintf(formattedObjectmapName, MAX_LEVEL_NAME_LENGTH, "assets/maps/%s.cmom", level);
    snprintf(currentLevelName, MAX_LEVEL_NAME_LENGTH, "assets/maps/%s", level);

    Tilemap::load(formattedTilemapName);
    Objects::clear();
    Objects::load(formattedObjectmapName);

    levelIndex = (levelIndex + 1) % levelNames.size();
}

void Game::tick() {
    if (paused) {
        if (singleStep) {
            singleStep = false;
        } else {
            return;
        }
    }
    RenderState::tick();

    if (Input::getButton(ButtonType::SWITCH).pressedFirstFrame) {
        Player::toggleWorld();
        RenderState::addRandomizedShake(2.0f);
        RenderState::startMixing();
        RenderState::startGlowing();
        Objects::instantiateClone(testParticleSystem, Player::getPosition());
    }

    if (tilemapEditor) {
        tilemapEditor->tick(Window::SECONDS_PER_TICK);
        if (Input::getButton(ButtonType::ABILITY).pressedFirstFrame) {
            tilemapEditor->flush();
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
    } else {
        Objects::tick();
        Player::tick();
    }
}

void Game::render(float lag) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    RenderState::bindAndClearDefaultFramebuffer();
    RenderState::updateViewMatrix(lag);
    if (tilemapEditor) {
        tilemapEditor->render();
        return;
    }
    RenderState::prepareEffectFramebuffer();
    Player::render(lag);
    Tilemap::render();

    RenderState::enableBlending();
    ParticleRenderer::prepare();
    Objects::render(lag);
    ParticleRenderer::render();
    RenderState::disableBlending();
    RenderState::renderEffects(lag);
}

void Game::renderImGui() {
    if (tilemapEditor) {
        return;
    }

    ImGui::Begin("DevGUI");

    if (ImGui::Button("Next level")) {
        nextLevel();
    }

    ImGui::SameLine();
    if (ImGui::Button(paused ? "Unpause" : "Pause")) {
        paused = !paused;
    }

    if (paused && ImGui::Button("Next frame")) {
        singleStep = true;
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
        snprintf(tileMapName, MAX_LEVEL_NAME_LENGTH, "%s.cmtm", currentLevelName);
        static char objectMapName[MAX_LEVEL_NAME_LENGTH];
        snprintf(objectMapName, MAX_LEVEL_NAME_LENGTH, "%s.cmom", currentLevelName);

        if (ImGui::Button("Load")) {
            Tilemap::load(tileMapName);
            Objects::clear();
            Objects::load(objectMapName);
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

        const char* destructionInfo = object->shouldDestroy ? "[QUEUED FOR DESTRUCTION] " : "";

        char header[256];
        snprintf(header, 256, "%s#%zu: %s (prototype #%d)", destructionInfo, i,
                 object->getTypeName(), object->prototypeId);

        ImGui::PushID(header);
        if (ImGui::CollapsingHeader(header)) {
            ImGui::DragFloat2("Position", object->position.data());
            ImGui::InputInt("Prototype ID (dangerous)", &object->prototypeId);
            ImGui::Spacing();

            object->renderImGui();
            if (ImGui::Button("Destroy")) {
                object->destroy();
            }

            if (ImGui::Button("Save")) {
                Objects::saveObject(objectSaveLocation, *object);
            }
            ImGui::SameLine();
            ImGui::InputText("##loc", objectSaveLocation, MAX_LEVEL_NAME_LENGTH);
        }
        ImGui::PopID();
    }

    ImGui::End();
}

void Game::onWindowResize(int width, int height) {
    glViewport(0, 0, width, height);
    if (tilemapEditor) {
        tilemapEditor->onScreenResize(width, height);
    }
    RenderState::resize(width, height);
}

void Game::onMouseEvent(void* eventPointer) {
    if (tilemapEditor) {
        tilemapEditor->onMouseEvent(eventPointer);
    }
}
