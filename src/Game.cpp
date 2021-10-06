#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <imgui.h>
#include <string>
#include <vector>

#include "Clock.h"
#include "Game.h"
#include "Input.h"
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

static std::vector<const char*> levelNames = {"map0", "map1"};
static int levelIndex = 0;
// TODO: make the level list configurable in the UI and get rid of this
static char currentLevelName[MAX_LEVEL_NAME_LENGTH] = "assets/maps/map0";
static char objectSaveLocation[MAX_LEVEL_NAME_LENGTH] = "assets/subdir/object.cmob";
static char objectLoadLocation[MAX_LEVEL_NAME_LENGTH] = "assets/subdir/object.cmob";

static TilemapEditor* tilemapEditor;
static bool paused;
static bool singleStep;

static int fade = 0;
static int fadeAdd = 0;

static Clock tps;
static Clock fps;

bool Game::init() {
    Tiles::init();
    if (Tilemap::init(48, 27) || Objects::init() || TilemapEditor::init() || RenderState::init() ||
        ParticleRenderer::init() || Font::init() || TextureRenderer::init() || Player::init()) {
        return true;
    }

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

    if (Input::getButton(ButtonType::SWITCH).pressedFirstFrame) {
        Player::toggleWorld();
        RenderState::addRandomizedShake(1.0f);
        RenderState::startMixing();
        RenderState::startGlowing();

        SoundManager::playSoundEffect(Sound::WORLD_SWITCH);
        SoundManager::switchMusic();
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
    fps.update();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    RenderState::bindAndClearDefaultFramebuffer();
    if (tilemapEditor) {
        RenderState::updateEditorViewMatrix(lag);
        tilemapEditor->render();
        return;
    }
    RenderState::updateViewMatrix(lag);
    RenderState::prepareEffectFramebuffer();
    Tilemap::renderBackground();
    Player::render(lag);

    RenderState::enableBlending();
    ParticleRenderer::prepare();
    Objects::render(lag); // Particles don't render if we don't do this for some reason
    ParticleRenderer::render();
    RenderState::disableBlending();

    Tilemap::render();

    RenderState::enableBlending();
    Objects::render(lag);
    RenderState::disableBlending();
    RenderState::renderEffects(lag);

    RenderState::enableBlending();

    Font::prepare();
    char buffer[256];
    snprintf(buffer, 256, "FPS: %2.0f TPS: %3.0f", fps.getUpdatesPerSecond(),
             tps.getUpdatesPerSecond());
    Font::draw(Vector(0.0f, 0.0f), 2.0f, ColorUtils::RED, buffer);

    TextureRenderer::render(lag);

    ObjectRenderer::prepare(Matrix());
    ObjectRenderer::drawRectangle(Vector(-1.0f, -1.0f), Vector(2.0f, 2.0f),
                                  ColorUtils::setAlpha(ColorUtils::BLACK, fade));
    RenderState::disableBlending();
}

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

        const char* destructionInfo = "";
        if (!object->destroyOnLevelLoad) {
            destructionInfo = "[PERSISTENT] ";
        }
        if (object->shouldDestroy) {
            destructionInfo = "[QUEUED FOR DESTRUCTION] ";
        }

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

void Game::fadeIn(int speed) {
    fadeAdd = -speed;
}

void Game::fadeOut(int speed) {
    fadeAdd = speed;
}

bool Game::isFading() {
    return fade != 0 && fade != 255;
}
