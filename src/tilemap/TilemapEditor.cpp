#include "TilemapEditor.h"

#include "objects/ColorObject.h"

void STBTE_DRAW_RECT(int x0, int y0, int x1, int y1, unsigned int color);
void STBTE_DRAW_TILE(int x0, int y0, unsigned short id, int highlight, float* data);

static int getPropType(int n, short* tiledata, float* params);
static const char* getPropName(int n, short* tiledata, float* params);
static float getPropMin(int n, short* tiledata, float* params);
static float getPropMax(int n, short* tiledata, float* params);

#define STBTE_PROP_TYPE getPropType
#define STBTE_PROP_NAME getPropName
#define STBTE_PROP_MIN getPropMin
#define STBTE_PROP_MAX getPropMax

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
#include "objects/Objects.h"
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

    for (size_t i = 0; i < Tiles::getCount(); i++) {
        const char* group = Tiles::get(i).getEditorGroup();
        if (group != nullptr) {
            stbte_define_tile(stbTileMap, i, 0xFFFFFFFF, group);
        }
    }

    for (size_t i = 0; i < Objects::getPrototypeCount(); i++) {
        stbte_define_tile(stbTileMap, 1000 + i, 0xFFFFFFFF, "Objects");
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

    for (auto& object : Objects::getObjects()) {
        int x = object->position.x;
        int y = object->position.y;
        stbte_set_tile(stbTileMap, x, y, 0, object->prototypeId + 1000);

        if (object->prototypeId == 0) {
            auto colorObject = std::dynamic_pointer_cast<ColorObject>(object);
            stbte_set_property(stbTileMap, x, y, 0, colorObject->data.size.x);
            stbte_set_property(stbTileMap, x, y, 1, colorObject->data.size.y);
            stbte_set_property(stbTileMap, x, y, 2, (int)colorObject->data.abilities[0]);
            stbte_set_property(stbTileMap, x, y, 3, (int)colorObject->data.abilities[1]);
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
    } else if (id < 1000) {
        Tiles::get(id).renderEditor(tilemapBuffer, tileSpaceX, tileSpaceY);
    } else {
        // IDs >= 1000 identify object prototypes
        int prototypeIndex = id - 1000;
        auto prototype = Objects::getPrototype(prototypeIndex)->clone();
        prototype->position = Vector(tileSpaceX, tileSpaceY);
        prototype->render(0.f);
    }

    Tilemap::renderBuffer(tilemapBuffer);
}

static int getPropType(int n, short* tiledata, float* params) {
    short tile = tiledata[0];
    if (tile < 1000) {
        return 0;
    }

    int prototypeIndex = tile - 1000;
    if (prototypeIndex == 0) { // ColorObject
        switch (n) {
            case 0: return STBTE_PROP_float;
            case 1: return STBTE_PROP_float;
            case 2: return STBTE_PROP_int;
            case 3: return STBTE_PROP_int;
        }
    }

    return 0;
}

static const char* getPropName(int n, short* tiledata, float* params) {
    short tile = tiledata[0];
    if (tile < 1000) {
        return "";
    }

    int prototypeIndex = tile - 1000;
    if (prototypeIndex == 0) { // ColorObject
        switch (n) {
            case 0: return "Size X";
            case 1: return "Size Y";
            case 2: return "Ability 1";
            case 3: return "Ability 2";
        }
    }

    return "";
}

static float getPropMin(int n, short* tiledata, float* params) {
    short tile = tiledata[0];
    if (tile < 1000) {
        return 0.f;
    }
    if (n >= 2) {
        return 0.f;
    }
    return 1.f;
}

static float getPropMax(int n, short* tiledata, float* params) {
    short tile = tiledata[0];
    if (tile < 1000) {
        return 0.f;
    }

    if (n >= 2) {
        return 5.f;
    }
    return 100.f;
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

    Objects::clear();

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            auto tiles = stbte_get_tile(stbTileMap, x, y);
            short id = tiles[0];

            if (id < 1000) {
                Tilemap::setTile(x, y, Tiles::get(id));
            } else {
                int prototypeIndex = id - 1000;
                auto obj = Objects::instantiateObject(prototypeIndex);
                obj->position = Vector(x, y);

                /*if (prototypeIndex == 0) {
                    auto colorObject = std::dynamic_pointer_cast<ColorObject>(obj);
                    float* props = stbte_get_properties(stbTileMap, x, y);
                    colorObject->data.size.x = props[0];
                    colorObject->data.size.y = props[1];
                    colorObject->data.abilities[0] = (Ability)props[2];
                    colorObject->data.abilities[1] = (Ability)props[3];
                }*/
            }
        }
    }
}
