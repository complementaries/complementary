#include "TilemapEditor.h"

void STBTE_DRAW_RECT(int x0, int y0, int x1, int y1, unsigned int color);
void STBTE_DRAW_TILE(int x0, int y0, unsigned short id, int highlight, float* data);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
#define STB_TILEMAP_EDITOR_IMPLEMENTATION
#include <SDL.h>
#define _SDL_H
#include <stb_tilemap_editor.h>
#undef STB_TILEMAP_EDITOR_IMPLEMENTATION
#pragma GCC diagnostic pop

#include "Tilemap.h"
#include "Tiles.h"
#include "objects/ObjectRenderer.h"

TilemapEditor::TilemapEditor(int screenWidth, int screenHeight) {
    stbTileMap = stbte_create_map(Tilemap::getWidth(), Tilemap::getHeight(), 1,
                                  screenWidth / Tilemap::getWidth(),
                                  screenHeight / Tilemap::getHeight(), 255);

    for (size_t i = 1; i < Tiles::getCount(); i++) {
        stbte_define_tile(stbTileMap, i, 0xFFFFFFFF, "Default");
    }

    stbte_set_display(0, 0, screenWidth, screenHeight);
    stbte_set_background_tile(stbTileMap, 0);

    for (int y = 0; y < Tilemap::getHeight(); y++) {
        for (int x = 0; x < Tilemap::getWidth(); x++) {
            auto& tile = Tilemap::getTile(x, y);
            if (tile.getId() != Tiles::AIR.getId()) {
                stbte_set_tile(stbTileMap, x, y, 0, tile.getId());
            }
        }
    }
}

TilemapEditor::~TilemapEditor() {
    free(stbTileMap);
}

void STBTE_DRAW_RECT(int x0, int y0, int x1, int y1, unsigned int color) {
    const int screenWidth = 850;
    const int screenHeight = 480;
    float x0F = (float)x0 * Tilemap::getWidth() / screenWidth;
    float x1F = (float)x1 * Tilemap::getWidth() / screenWidth;
    float y0F = (float)y0 * Tilemap::getHeight() / screenHeight;
    float y1F = (float)y1 * Tilemap::getHeight() / screenHeight;

    ObjectRenderer::drawRectangle(Vector(x0F, y0F), Vector(x1F - x0F, y1F - y0F), color);
}

void STBTE_DRAW_TILE(int x0, int y0, unsigned short id, int highlight, float* data) {
    (void)highlight;
    (void)data;
    (void)id;

    const int screenWidth = 850;
    const int screenHeight = 480;
    float tileSpaceX = (float)x0 * Tilemap::getWidth() / screenWidth;
    float tileSpaceY = (float)y0 * Tilemap::getHeight() / screenHeight;

    static Buffer tilemapBuffer;
    tilemapBuffer.clear();

    if (id == Tiles::AIR.getId()) {
        float minX = tileSpaceX;
        float minY = tileSpaceY;
        float maxX = minX + 1.0f;
        float maxY = minY + 1.0f;
        Color color = ColorUtils::invert(Tiles::WALL.getColor());
        tilemapBuffer.add(minX).add(minY).add(color);
        tilemapBuffer.add(maxX).add(minY).add(color);
        tilemapBuffer.add(minX).add(maxY).add(color);
        tilemapBuffer.add(maxX).add(maxY).add(color);
        tilemapBuffer.add(maxX).add(minY).add(color);
        tilemapBuffer.add(minX).add(maxY).add(color);
    } else {
        Tiles::get(id).render(tilemapBuffer, tileSpaceX, tileSpaceY);
    }

    Tilemap::renderBuffer(tilemapBuffer);
}

void TilemapEditor::tick(float dt) {
    stbte_tick(stbTileMap, dt);
}

void TilemapEditor::render() {
    ObjectRenderer::prepare();
    stbte_draw(stbTileMap);
}

void TilemapEditor::onMouseEvent(void* eventPtr) {
    stbte_mouse_sdl(stbTileMap, eventPtr, 1.f, 1.f, 0, 0);
}

void TilemapEditor::flush() {
    int width, height;
    stbte_get_dimensions(stbTileMap, &width, &height);

    Tilemap::setWidth(width);
    Tilemap::setHeight(height);
    Tilemap::reset();

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            auto tiles = stbte_get_tile(stbTileMap, x, y);
            short tile = tiles[0];

            Tilemap::setTile(x, y, Tiles::get(tile));
        }
    }
}
