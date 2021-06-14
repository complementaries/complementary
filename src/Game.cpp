#include <cstdlib>

#include "Game.h"
#include "Player.h"
#include "graphics/gl/Glew.h"
#include "graphics/gl/Shader.h"
#include "graphics/gl/VertexBuffer.h"
#include "math/Matrix.h"
#include "tilemap/Tilemap.h"
#include "tilemap/Tiles.h"

static Tilemap tilemap;
static Player player;
Matrix Game::viewMatrix;

bool Game::init() {
    Tiles::init();
    if (tilemap.init(32, 18) || player.init()) {
        return true;
    }
    for (int x = 0; x < tilemap.getWidth(); x++) {
        for (int y = 0; y < tilemap.getHeight(); y++) {
            tilemap.setTile(x, y, rand() % 20 ? Tiles::AIR : Tiles::WALL);
        }
    }
    return false;
}

void Game::tick() {
    player.tick(tilemap);
}

void Game::render(float lag) {
    glClear(GL_COLOR_BUFFER_BIT);
    MatrixUtils::setTransform(tilemap.getWidth(), tilemap.getHeight(), viewMatrix);
    tilemap.render();
    player.render(lag);
}