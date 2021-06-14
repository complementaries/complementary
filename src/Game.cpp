#include "Game.h"

#include <iostream>

#include "Input.h"
#include "graphics/gl/Glew.h"
#include "graphics/gl/Shader.h"
#include "graphics/gl/VertexBuffer.h"
#include "tilemap/Tilemap.h"
#include "tilemap/Tiles.h"

static Tilemap tilemap;

bool Game::init() {
    Tiles::init();
    if (tilemap.init(16, 9)) {
        return true;
    }
    for (int x = 0; x < tilemap.getWidth(); x++) {
        for (int y = 0; y < tilemap.getHeight(); y++) {
            tilemap.setTile(x, y, rand() % 3 ? Tiles::AIR : Tiles::WALL);
        }
    }
    return false;
}

void Game::tick() {
    // x += Input::getHorizontal() * 0.01f;
}

void Game::render(float lag) {
    (void)lag;
    glClear(GL_COLOR_BUFFER_BIT);
    tilemap.render();
}