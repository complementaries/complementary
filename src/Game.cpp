#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <imgui.h>

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

static char levelName[50] = "assets/maps/map0.cmtm";
static char objectMapName[50] = "assets/maps/map0.cmom";

static TilemapEditor* tilemapEditor;

bool Game::init() {
    Tiles::init();
    if (Tilemap::init(48, 27) || Player::init() || Objects::init() || Particles::init()) {
        return true;
    }
    Objects::add(new ColorObject(Vector(8.0f, 21.5f), Vector(3.0f, 0.5f), Ability::WALL_JUMP,
                                 Ability::DASH));
    Objects::add(new ColorObject(Vector(10.0f, 26.5f), Vector(3.0f, 0.5f), Ability::GLIDER,
                                 Ability::DOUBLE_JUMP));

    return false;
}

void Game::tick() {
    if (tilemapEditor) {
        tilemapEditor->tick(Window::SECONDS_PER_TICK);
    } else {
        Objects::tick();
        Player::tick();
        Particles::tick();
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
    Particles::render(lag);

    if (tilemapEditor) {
        tilemapEditor->render();
    }
}

void Game::renderImGui() {
    ImGui::Begin("DevGUI");

    if (tilemapEditor) {
        if (ImGui::Button("Close Editor")) {
            tilemapEditor->flush();
            delete tilemapEditor;
            tilemapEditor = nullptr;
        }

        int zoom = tilemapEditor->getZoom();
        ImGui::SliderInt("Zoom", &zoom, 1, 3);
        tilemapEditor->setZoom(zoom);

        ImGui::End();
        return;
    }

    if (ImGui::CollapsingHeader("Tilemap")) {
        ImGui::Text("Width: %d, Height: %d", Tilemap::getWidth(), Tilemap::getHeight());

        if (!tilemapEditor && ImGui::Button("Open Editor")) {
            tilemapEditor = new TilemapEditor(Window::getWidth(), Window::getHeight());
        }

        ImGui::InputText("Level name", levelName, 30);

        if (ImGui::Button("Load")) {
            Tilemap::load(levelName);
        }

        ImGui::SameLine();
        if (ImGui::Button("Save")) {
            Tilemap::save(levelName);
        }

        ImGui::InputText("Object map name", objectMapName, 30);
        if (ImGui::Button("Save objects")) {
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
