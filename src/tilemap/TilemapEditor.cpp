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
#include <graphics/gl/Glew.h>

static int globalScreenWidth;
static int globalScreenHeight;
static float globalZoom = 2.f;

const int OBJECT_ID_OFFSET = 1000;

static int getZoomedWidth() {
    return (int)(globalScreenWidth / globalZoom);
}

static int getZoomedHeight() {
    return (int)(globalScreenHeight / globalZoom);
}

TilemapEditor::TilemapEditor(int screenWidth, int screenHeight) {
    globalScreenWidth = screenWidth;
    globalScreenHeight = screenHeight;
    stbTileMap = stbte_create_map(Tilemap::getWidth(), Tilemap::getHeight(), 2,
                                  getZoomedWidth() / Tilemap::getWidth(),
                                  getZoomedHeight() / Tilemap::getHeight(), 255);

    stbTileMap->propmode = STBTE__propmode_always;

    stbte_set_layername(stbTileMap, 0, "Tiles");
    stbte_set_layername(stbTileMap, 1, "Objects");

    for (size_t i = 0; i < Tiles::getCount(); i++) {
        const char* group = Tiles::get(i).getEditorGroup();
        if (group != nullptr) {
            stbte_define_tile(stbTileMap, i, 1, group);
        }
    }

    for (size_t i = 0; i < Objects::getPrototypeCount(); i++) {
        stbte_define_tile(stbTileMap, OBJECT_ID_OFFSET + i, 1 << 1, "Objects");
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
        if (object->prototypeId < 0 ||
            object->prototypeId >= static_cast<int>(Objects::getPrototypeCount())) {
            fprintf(stderr,
                    "Encountered an object with an invalid prototype ID, it will be deleted after "
                    "editing!\n");
            continue;
        }
        int x = object->position.x;
        int y = object->position.y;
        stbte_set_tile(stbTileMap, x, y, 1, object->prototypeId + OBJECT_ID_OFFSET);

        auto props = object->getTileEditorProps();
        stbte_set_property(stbTileMap, x, y, 0, 1);
        for (size_t i = 0; i < props.size() && i < STBTE_MAX_PROPERTIES; i++) {
            stbte_set_property(stbTileMap, x, y, i + 1, props[i].value);
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
        Tilemap::renderBuffer(tilemapBuffer);
    } else if (id < OBJECT_ID_OFFSET) {
        Tiles::get(id).renderEditor(tilemapBuffer, tileSpaceX, tileSpaceY);
        Tilemap::renderBuffer(tilemapBuffer);
    } else {
        // IDs >= 1000 identify object prototypes
        int prototypeIndex = id - OBJECT_ID_OFFSET;
        auto prototype = Objects::getPrototype(prototypeIndex)->clone();
        prototype->position = Vector(tileSpaceX, tileSpaceY);
        if (data != nullptr && data[0] != 0) {
            // The first prop is a boolean which stores whether the props have been initialized
            prototype->applyTileEditorData(data + 1);
        }

        // TODO: do we need this for all objects? should the object itself set the blend mode?
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);
        prototype->render(0.f);
        glDisable(GL_BLEND);
    }
}

static int getPropType(int n, short* tiledata, float* params) {
    short tile = tiledata[1];
    if (tile < OBJECT_ID_OFFSET) {
        return 0;
    }

    int prototypeIndex = tile - OBJECT_ID_OFFSET;
    auto props = Objects::getPrototype(prototypeIndex)->getTileEditorProps();

    if (n == 0) {
        // The first prop is set to 1 if the props have been initialized with the prototype's
        // default values. If not, initialize them here since there's no function that is called
        // when a prototype is placed.
        if (params[0] == 0) {
            for (size_t i = 0; i < props.size() && i < STBTE_MAX_PROPERTIES - 1; i++) {
                params[i + 1] = props[i].value;
            }
            params[0] = 1;
        }
        return 0;
    }

    if (props.size() <= static_cast<size_t>(n - 1)) {
        return 0;
    }

    int type = 0;
    switch (props[n - 1].type) {
        case TileEditorPropType::INT: type = STBTE_PROP_int; break;
        case TileEditorPropType::FLOAT: type = STBTE_PROP_float; break;
        case TileEditorPropType::BOOL: type = STBTE_PROP_bool; break;
    }
    if (props[n - 1].disabled) {
        type |= STBTE_PROP_disabled;
    }

    return type;
}

static const char* getPropName(int n, short* tiledata, float* params) {
    short tile = tiledata[1];
    if (tile < OBJECT_ID_OFFSET) {
        return "";
    }

    int prototypeIndex = tile - OBJECT_ID_OFFSET;
    auto props = Objects::getPrototype(prototypeIndex)->getTileEditorProps();
    if (props.size() <= static_cast<size_t>(n - 1)) {
        return "";
    }

    return props[n - 1].name;
}

static float getPropMin(int n, short* tiledata, float* params) {
    short tile = tiledata[1];
    if (tile < OBJECT_ID_OFFSET) {
        return 0.f;
    }

    int prototypeIndex = tile - OBJECT_ID_OFFSET;
    auto props = Objects::getPrototype(prototypeIndex)->getTileEditorProps();
    if (props.size() <= static_cast<size_t>(n - 1)) {
        return 0;
    }

    return props[n - 1].min;
}

static float getPropMax(int n, short* tiledata, float* params) {
    short tile = tiledata[1];
    if (tile < OBJECT_ID_OFFSET) {
        return 0.f;
    }

    int prototypeIndex = tile - OBJECT_ID_OFFSET;
    auto props = Objects::getPrototype(prototypeIndex)->getTileEditorProps();
    if (props.size() <= static_cast<size_t>(n - 1)) {
        return 0;
    }

    return props[n - 1].max;
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
            short tileId = tiles[0]; // Tile layer
            Tilemap::setTile(x, y, Tiles::get(tileId));

            short objectId = tiles[1]; // Object layer
            if (objectId >= OBJECT_ID_OFFSET) {
                int prototypeIndex = objectId - OBJECT_ID_OFFSET;
                auto obj = Objects::instantiateObject(prototypeIndex);
                obj->position = Vector(x, y);

                float* props = stbte_get_properties(stbTileMap, x, y);
                obj->applyTileEditorData(props + 1);

                // initTileEditorData() was already called with the prototype's values, overwrite
                // them with the new values
                obj->getTileEditorProps().clear();
                obj->initTileEditorData(obj->getTileEditorProps());
            }
        }
    }
}
