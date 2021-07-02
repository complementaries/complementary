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

static Tilemap tilemap;
Matrix Game::viewMatrix;

bool Game::init() {
    Tiles::init();
    if (tilemap.init(32, 18) || Player::init() || Objects::init()) {
        return true;
    }
    for (int x = 0; x < tilemap.getWidth(); x++) {
        for (int y = 0; y < tilemap.getHeight(); y++) {
            tilemap.setTile(x, y, rand() % 20 ? Tiles::AIR : Tiles::WALL);
        }
    }
    tilemap.setTile(5, tilemap.getHeight() - 1, Tiles::SPIKES);
    Objects::add(new ColorObject(Vector(5.0f, 12.5f), Vector(4.0f, 0.5f),
                                 ColorUtils::rgba(0xFF, 0xFF, 0x00)));
    return false;
}

void Game::tick() {
    Objects::tick();
    Player::tick(tilemap);
}

void Game::render(float lag) {
    glClear(GL_COLOR_BUFFER_BIT);
    MatrixUtils::setTransform(tilemap.getWidth(), tilemap.getHeight(), viewMatrix);
    tilemap.render();
    Objects::render(lag);
    Player::render(lag);
}

void Game::renderImGui() {
    ImGui::Begin("Tilemap");
    ImGui::Text("Width: %d, Height: %d", tilemap.getWidth(), tilemap.getHeight());
    ImGui::End();

    Player::renderImGui();
}
