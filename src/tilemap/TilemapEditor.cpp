#include "TilemapEditor.h"

void STBTE_DRAW_RECT(int x0, int y0, int x1, int y1, unsigned int color);
void STBTE_DRAW_TILE(int x0, int y0, unsigned short id, int highlight, float* data);

static int tilemapBackgroundColor;
static int globalScreenWidth;
static int globalScreenHeight;
static float globalZoom = 1.f;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
#define STB_TILEMAP_EDITOR_IMPLEMENTATION
#include <SDL.h>
#define _SDL_H
#include <cstdio>
#define STBTE_COLOR_TILEMAP_BACKGROUND tilemapBackgroundColor
#include <stb_tilemap_editor.h>
#undef STB_TILEMAP_EDITOR_IMPLEMENTATION
#pragma GCC diagnostic pop

#include "Tilemap.h"
#include "Tiles.h"
#include "objects/ObjectRenderer.h"
#include "player/Player.h"

static int getZoomedWidth() {
    return (int)(globalScreenWidth / globalZoom);
}

static int getZoomedHeight() {
    return (int)(globalScreenHeight / globalZoom);
}

TilemapEditor::TilemapEditor(int screenWidth, int screenHeight) {
    globalScreenWidth = screenWidth;
    globalScreenHeight = screenHeight;
    stbTileMap = stbte_create_map(Tilemap::getWidth(), Tilemap::getHeight(), 1,
                                  getZoomedWidth() / Tilemap::getWidth(),
                                  getZoomedHeight() / Tilemap::getHeight(), 255);

    for (size_t i = 1; i < Tiles::getCount(); i++) {
        stbte_define_tile(stbTileMap, i, 0xFFFFFFFF, "Default");
    }

    stbte_set_display(0, 0, getZoomedWidth(), getZoomedHeight());

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
    float x0F = (float)x0 * Tilemap::getWidth() / getZoomedWidth();
    float x1F = (float)x1 * Tilemap::getWidth() / getZoomedWidth();
    float y0F = (float)y0 * Tilemap::getHeight() / getZoomedHeight();
    float y1F = (float)y1 * Tilemap::getHeight() / getZoomedHeight();

    ObjectRenderer::drawRectangle(Vector(x0F, y0F), Vector(x1F - x0F, y1F - y0F), color);
}

void STBTE_DRAW_TILE(int x0, int y0, unsigned short id, int highlight, float* data) {
    (void)highlight;
    (void)data;
    (void)id;

    float tileSpaceX = (float)x0 * Tilemap::getWidth() / getZoomedWidth();
    float tileSpaceY = (float)y0 * Tilemap::getHeight() / getZoomedHeight();

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
    tilemapBackgroundColor = Player::invertColors() ? 0x000000 : 0xffffff;
    stbte_tick(stbTileMap, dt);
}

void TilemapEditor::render() {
    ObjectRenderer::prepare();
    stbte_draw(stbTileMap);
}

void TilemapEditor::onMouseEvent(void* eventPtr) {
    stbte_mouse_sdl(stbTileMap, eventPtr, 1.f / globalZoom, 1.f / globalZoom, 0, 0);
}

void TilemapEditor::onScreenResize(int width, int height) {
    globalScreenWidth = width;
    globalScreenHeight = height;
    stbte_set_display(0, 0, getZoomedWidth(), getZoomedHeight());
}

void TilemapEditor::setZoom(float zoom) {
    globalZoom = zoom;
    onScreenResize(globalScreenWidth, globalScreenHeight);
}

float TilemapEditor::getZoom() {
    return globalZoom;
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
