#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <imgui.h>
#include <string>
#include <vector>

#include "Game.h"
#include "Input.h"
#include "graphics/Buffer.h"
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
#include "particles/Particles.h"
#include "player/Player.h"
#include "tilemap/Tilemap.h"
#include "tilemap/TilemapEditor.h"
#include "tilemap/Tiles.h"

Matrix Game::viewMatrix;

static constexpr int MAX_LEVEL_NAME_LENGTH = 100;

static std::vector<const char*> levelNames = {"map0", "map1"};
static int levelIndex = 0;
// TODO: make the level list configurable in the UI and get rid of this
static char currentLevelName[MAX_LEVEL_NAME_LENGTH] = "assets/maps/map0";

static TilemapEditor* tilemapEditor;

bool Game::init() {
    Tiles::init();
    if (Tilemap::init(48, 27) || Player::init() || Objects::init() || Particles::init()) {
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

    if (snprintf(formattedTilemapName, MAX_LEVEL_NAME_LENGTH, "assets/maps/%s.cmtm", level) > 99) {
        puts("The level file name is too long!");
    }
    snprintf(formattedObjectmapName, MAX_LEVEL_NAME_LENGTH, "assets/maps/%s.cmom", level);
    snprintf(currentLevelName, MAX_LEVEL_NAME_LENGTH, "assets/maps/%s", level);

    Tilemap::load(formattedTilemapName);
    Objects::load(formattedObjectmapName);

    levelIndex = (levelIndex + 1) % levelNames.size();
}

void Game::tick() {
    if (Input::getButton(ButtonType::SWITCH).pressedFirstFrame) {
        Player::toggleWorld();
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
    if (Player::invertColors()) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    } else {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    }
    glClear(GL_COLOR_BUFFER_BIT);
    viewMatrix.unit()
        .transform(Vector(-1.0f, 1.0f))
        .scale(Vector(2.0f / Tilemap::getWidth(), -2.0f / Tilemap::getHeight()));
    Player::render(lag);
    Tilemap::render();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    Objects::render(lag);
    glDisable(GL_BLEND);

    if (tilemapEditor) {
        tilemapEditor->render();
    }
}

void Game::renderImGui() {
    if (tilemapEditor) {
        return;
    }

    ImGui::Begin("DevGUI");

    if (ImGui::Button("Next level")) {
        nextLevel();
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
            Objects::load(objectMapName);
        }

        ImGui::SameLine();
        if (ImGui::Button("Save")) {
            Tilemap::save(tileMapName);
            Objects::save(objectMapName);
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

    ImGui::End();
}

void Game::onWindowResize(int width, int height) {
    glViewport(0, 0, width, height);
    if (tilemapEditor) {
        tilemapEditor->onScreenResize(width, height);
    }
}

void Game::onMouseEvent(void* eventPointer) {
    if (tilemapEditor) {
        tilemapEditor->onMouseEvent(eventPointer);
    }
}
