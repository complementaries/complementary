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
#include "objects/ObjectRenderer.h"
#include "objects/Objects.h"
#include "particles/Particles.h"
#include "player/Player.h"
#include "tilemap/Tilemap.h"
#include "tilemap/TilemapEditor.h"
#include "tilemap/Tiles.h"

Matrix Game::viewMatrix;

static std::vector<const char*> levelNames = {"map0", "map1"};
static int levelIndex = 0;
// TODO: make the level list configurable in the UI and get rid of this
static char currentLevelName[100] = "assets/maps/map0";

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

    char formattedTilemapName[100];
    char formattedObjectmapName[100];

    if (snprintf(formattedTilemapName, 100, "assets/maps/%s.cmtm", level) > 99) {
        puts("The level file name is too long!");
    }
    snprintf(formattedObjectmapName, 100, "assets/maps/%s.cmom", level);
    snprintf(currentLevelName, 100, "assets/maps/%s", level);

    Tilemap::load(formattedTilemapName);
    Objects::load(formattedObjectmapName);

    levelIndex = (levelIndex + 1) % levelNames.size();
}

void Game::tick() {
    if (tilemapEditor) {
        tilemapEditor->tick(Window::SECONDS_PER_TICK);
        if (Input::getButton(ButtonType::ABILITY).pressedFirstFrame) {
            tilemapEditor->flush();
            delete tilemapEditor;
            tilemapEditor = nullptr;
        }

        float zoom = tilemapEditor->getZoom();
        if (Input::getButton(ButtonType::RIGHT).pressed) {
            zoom += 0.02f;
        }
        if (Input::getButton(ButtonType::LEFT).pressed) {
            zoom -= 0.02f;
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
    MatrixUtils::setTransform(Tilemap::getWidth(), Tilemap::getHeight(), viewMatrix);
    (void)lag;
    Tilemap::render();
    Objects::render(lag);
    Player::render(lag);

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

    if (ImGui::CollapsingHeader("Tilemap")) {
        ImGui::Text("Width: %d, Height: %d", Tilemap::getWidth(), Tilemap::getHeight());

        if (!tilemapEditor && ImGui::Button("Open Editor")) {
            tilemapEditor = new TilemapEditor(Window::getWidth(), Window::getHeight());
        }

        ImGui::InputText("Level name", currentLevelName, 50);
        static char tileMapName[60];
        sprintf(tileMapName, "%s.cmtm", currentLevelName);
        static char objectMapName[60];
        sprintf(objectMapName, "%s.cmom", currentLevelName);

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
