#include <cstdlib>
#include <ctime>
#include <imgui.h>

#include "Game.h"
#include "Input.h"
#include "graphics/gl/Glew.h"
#include "graphics/gl/Shader.h"
#include "graphics/gl/VertexBuffer.h"
#include "imgui/ImGuiUtils.h"
#include "math/Matrix.h"
#include "objects/ColorObject.h"
#include "objects/Objects.h"
#include "player/Player.h"
#include "tilemap/Tilemap.h"
#include "tilemap/Tiles.h"

Matrix Game::viewMatrix;

static char levelName[50] = "assets/maps/map0.cmtm";
static char objectMapName[50] = "assets/maps/map0.cmom";

bool Game::init() {
    Tiles::init();
    if (Tilemap::init(48, 27) || Player::init() || Objects::init()) {
        return true;
    }
    srand(time(nullptr));
    for (int x = 0; x < Tilemap::getWidth(); x++) {
        for (int y = 0; y < Tilemap::getHeight(); y++) {
            Tilemap::setTile(x, y, rand() % 20 ? Tiles::AIR : Tiles::WALL);
        }
    }
    Tilemap::setTile(15, Tilemap::getHeight() - 1, Tiles::SPIKES);
    Objects::add(new ColorObject(Vector(5.0f, 12.5f), Vector(4.0f, 5.5f), Ability::WALL_JUMP,
                                 Ability::DASH));
    Objects::add(new ColorObject(Vector(10.0f, 16.5f), Vector(3.0f, 0.5f), Ability::GLIDER,
                                 Ability::DOUBLE_JUMP));

    for (int x = 0; x < 7; x++) {
        for (int y = 0; y < Tilemap::getHeight() - 3; y++) {
            Tilemap::setTile(x, y, x != 6 ? Tiles::AIR : Tiles::WALL);
        }
    }

    return false;
}

void Game::tick() {
    Objects::tick();
    Player::tick();
}

void Game::render(float lag) {
    if (Player::invertColors()) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    } else {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    }
    glClear(GL_COLOR_BUFFER_BIT);
    MatrixUtils::setTransform(Tilemap::getWidth(), Tilemap::getHeight(), viewMatrix);
    Tilemap::render();
    Objects::render(lag);
    Player::render(lag);
}

void Game::renderImGui() {
    ImGui::Begin("DevGUI");
    if (ImGui::CollapsingHeader("Tilemap")) {
        ImGui::Text("Width: %d, Height: %d", Tilemap::getWidth(), Tilemap::getHeight());
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
