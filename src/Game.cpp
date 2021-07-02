#include <cstdlib>
#include <imgui.h>

#include "Game.h"
#include "graphics/gl/Glew.h"
#include "graphics/gl/Shader.h"
#include "graphics/gl/VertexBuffer.h"
#include "math/Matrix.h"
#include "objects/ColorObject.h"
#include "objects/Objects.h"
#include "player/Player.h"
#include "tilemap/Tilemap.h"
#include "tilemap/Tiles.h"

Matrix Game::viewMatrix;

bool Game::init() {
    Tiles::init();
    if (Tilemap::init(32, 18) || Player::init() || Objects::init()) {
        return true;
    }
    for (int x = 0; x < Tilemap::getWidth(); x++) {
        for (int y = 0; y < Tilemap::getHeight(); y++) {
            Tilemap::setTile(x, y, rand() % 20 ? Tiles::AIR : Tiles::WALL);
        }
    }
    Tilemap::setTile(5, Tilemap::getHeight() - 1, Tiles::SPIKES);
    Objects::add(new ColorObject(Vector(5.0f, 12.5f), Vector(4.0f, 0.5f), Ability::WALL_JUMP,
                                 Ability::DASH));
    Objects::add(new ColorObject(Vector(10.0f, 16.5f), Vector(3.0f, 0.5f), Ability::GLIDER,
                                 Ability::DOUBLE_JUMP));
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
    ImGui::Begin("Tilemap");
    ImGui::Text("Width: %d, Height: %d", Tilemap::getWidth(), Tilemap::getHeight());
    ImGui::End();

    Player::renderImGui();
}
