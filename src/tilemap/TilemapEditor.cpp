#ifndef NDEBUG
#include "TilemapEditor.h"

#include "graphics/Window.h"
#include "objects/ColorObject.h"

void STBTE_DRAW_RECT(int x0, int y0, int x1, int y1, unsigned int color);
void STBTE_DRAW_TILE(int x0, int y0, unsigned short id, int highlight, float* data);

static int getPropType(int n, short* tiledata, float* params);
static const char* getPropName(int n, short* tiledata, float* params);
static float getPropMin(int n, short* tiledata, float* params);
static float getPropMax(int n, short* tiledata, float* params);
static float getPropScale(int n, short* tiledata, float* params);

#define STBTE_MAX_PROPERTIES 12
#define STBTE_PROP_TYPE getPropType
#define STBTE_PROP_NAME getPropName
#define STBTE_PROP_MIN getPropMin
#define STBTE_PROP_MAX getPropMax
#define STBTE_PROP_FLOAT_SCALE getPropScale

static int tilemapBackgroundColor;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
#ifdef __linux__
#pragma GCC diagnostic ignored "-Waggressive-loop-optimizations"
#endif
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
#include "graphics/RenderState.h"
#include "graphics/gl/Shader.h"
#include "graphics/gl/VertexBuffer.h"
#include "objects/ObjectRenderer.h"
#include "objects/Objects.h"
#include "player/Player.h"
#include <graphics/gl/Glew.h>

static float globalZoom = 2.0f;
static constexpr int TILE_WIDTH = 12;
static constexpr int TILE_HEIGHT = 12;

const int OBJECT_ID_OFFSET = 1000;

static Buffer renderBuffer;
static GL::VertexBuffer buffer;
static GL::Shader shader;

struct QueueData {
    std::shared_ptr<ObjectBase> object;
    float zLayer;
    bool inPalette;
};

static std::vector<QueueData> objectQueue;
static float zLayer;

bool TilemapEditor::init() {
    if (shader.compile({"assets/shaders/editor.vs", "assets/shaders/editor.fs"})) {
        return true;
    }
    buffer.init(GL::VertexBuffer::Attributes().addVector3().addRGBA());
    return false;
}

TilemapEditor::TilemapEditor(int screenWidth, int screenHeight) {
    stbTileMap = stbte_create_map(Tilemap::getWidth(), Tilemap::getHeight(), 2, TILE_WIDTH,
                                  TILE_HEIGHT, 255);

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

    stbte_set_display(0, 0, Window::getWidth(), Window::getHeight());

    for (int y = 0; y < Tilemap::getHeight(); y++) {
        for (int x = 0; x < Tilemap::getWidth(); x++) {
            auto& tile = Tilemap::getTile(x, y);
            if (tile.getId() != Tiles::AIR.getId()) {
                stbte_set_tile(stbTileMap, x, y, 0, tile.getId());
            }
        }
    }

    Objects::reset();

    for (auto& object : Objects::getObjects()) {
        if (object->prototypeId < 0 ||
            object->prototypeId >= static_cast<int>(Objects::getPrototypeCount())) {
            fprintf(stderr,
                    "Encountered an object with an invalid prototype ID, it will be deleted after "
                    "editing!\n");
            continue;
        }
        if (!object->allowSaving() || !object->destroyOnLevelLoad) {
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
    float minX = static_cast<float>(x0) / TILE_WIDTH;
    float maxX = static_cast<float>(x1) / TILE_WIDTH;
    float minY = static_cast<float>(y0) / TILE_HEIGHT;
    float maxY = static_cast<float>(y1) / TILE_HEIGHT;

    renderBuffer.add(minX).add(minY).add(zLayer).add(color);
    renderBuffer.add(maxX).add(minY).add(zLayer).add(color);
    renderBuffer.add(minX).add(maxY).add(zLayer).add(color);
    renderBuffer.add(maxX).add(maxY).add(zLayer).add(color);
    renderBuffer.add(maxX).add(minY).add(zLayer).add(color);
    renderBuffer.add(minX).add(maxY).add(zLayer).add(color);
    zLayer -= 0.0002f;
}

void STBTE_DRAW_TILE(int x0, int y0, unsigned short id, int highlight, float* data) {
    (void)highlight;

    float tileSpaceX = (float)x0 / TILE_WIDTH;
    float tileSpaceY = (float)y0 / TILE_HEIGHT;

    if (id == Tiles::AIR.getId()) {
        float minX = tileSpaceX;
        float minY = tileSpaceY;
        float maxX = minX + 1.0f;
        float maxY = minY + 1.0f;
        Color color = ColorUtils::invert(Tiles::WALL.getColor());
        renderBuffer.add(minX).add(minY).add(zLayer).add(color);
        renderBuffer.add(maxX).add(minY).add(zLayer).add(color);
        renderBuffer.add(minX).add(maxY).add(zLayer).add(color);
        renderBuffer.add(maxX).add(maxY).add(zLayer).add(color);
        renderBuffer.add(maxX).add(minY).add(zLayer).add(color);
        renderBuffer.add(minX).add(maxY).add(zLayer).add(color);
    } else if (id < OBJECT_ID_OFFSET) {
        if (data == nullptr) {
            float minX = tileSpaceX;
            float minY = tileSpaceY;
            float maxX = minX + 1.0f;
            float maxY = minY + 1.0f;
            Color color = ColorUtils::invert(Tiles::WALL.getColor());
            renderBuffer.add(minX).add(minY).add(zLayer).add(color);
            renderBuffer.add(maxX).add(minY).add(zLayer).add(color);
            renderBuffer.add(minX).add(maxY).add(zLayer).add(color);
            renderBuffer.add(maxX).add(maxY).add(zLayer).add(color);
            renderBuffer.add(maxX).add(minY).add(zLayer).add(color);
            renderBuffer.add(minX).add(maxY).add(zLayer).add(color);
        }

        Tiles::get(id).renderEditor(renderBuffer, tileSpaceX, tileSpaceY, zLayer);
    } else {
        // IDs >= 1000 identify object prototypes
        int prototypeIndex = id - OBJECT_ID_OFFSET;
        auto prototype = Objects::getPrototype(prototypeIndex)->clone();
        prototype->position = Vector(tileSpaceX, tileSpaceY);
        if (data != nullptr && data[0] != 0) {
            // The first prop is a boolean which stores whether the props have been initialized
            prototype->applyTileEditorData(data + 1);
        }
        objectQueue.push_back({prototype, zLayer, data == nullptr});
    }
    zLayer -= 0.0002f;
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

static float getPropScale(int n, short* tiledata, float* params) {
    short tile = tiledata[1];
    if (tile < OBJECT_ID_OFFSET) {
        return 0.f;
    }

    int prototypeIndex = tile - OBJECT_ID_OFFSET;
    auto props = Objects::getPrototype(prototypeIndex)->getTileEditorProps();
    if (props.size() <= static_cast<size_t>(n - 1)) {
        return 0;
    }

    return props[n - 1].scale;
}

void TilemapEditor::tick(float dt) {
    tilemapBackgroundColor = Player::invertColors() ? 0x000000 : 0xffffff;
    stbte_tick(stbTileMap, dt);
}

static void patchTiledata() {
    float* data = const_cast<float*>(static_cast<const float*>(renderBuffer.getData()));
    int length = renderBuffer.getSize() / sizeof(float);
    for (int i = 0; i < length; i += 4) {
        data[i] *= TILE_WIDTH;
        data[i + 1] *= TILE_WIDTH;
    }
}

void TilemapEditor::render() {
    Matrix m;
    m.transform(Vector(-1.0f, 1.0f))
        .scale(Vector(2.0f / Window::getWidth(), -2.0f / Window::getHeight()));
    m.scale(Vector(globalZoom, globalZoom));

    renderBuffer.clear();
    zLayer = 0.0f;
    stbte_draw(stbTileMap);

    shader.use();
    shader.setMatrix("view", m);

    patchTiledata();

    int vertices = renderBuffer.getSize() / (sizeof(float) * 3 + 4);
    buffer.setStreamData(renderBuffer.getData(), renderBuffer.getSize());
    buffer.drawTriangles(vertices);

    RenderState::enableBlending();
    ObjectRenderer::bindBuffer(false);
    ObjectRenderer::dirtyStaticBuffer();
    ObjectRenderer::setScale(Vector(TILE_WIDTH, TILE_HEIGHT));
    for (unsigned int i = 0; i < objectQueue.size(); i++) {
        ObjectRenderer::setDefaultZ(objectQueue[i].zLayer);
        objectQueue[i].object->renderEditor(1.0f, objectQueue[i].inPalette);
    }
    ObjectRenderer::setScale(Vector(1.0f, 1.0f));
    ObjectRenderer::render(m);
    objectQueue.clear();
    RenderState::disableBlending();
}

void TilemapEditor::onMouseEvent(void* eventPtr) {
    stbte_mouse_sdl(stbTileMap, eventPtr, 1.f / globalZoom, 1.f / globalZoom, 0, 0);
}

void TilemapEditor::onScreenResize(int width, int height) {
    stbte_set_display(0, 0, Window::getWidth() / globalZoom, Window::getHeight() / globalZoom);
}

void TilemapEditor::setZoom(float zoom) {
    globalZoom = zoom;
    onScreenResize(0, 0);
}

float TilemapEditor::getZoom() {
    return globalZoom;
}

void TilemapEditor::flush() {
    int width, height;
    stbte_get_dimensions(stbTileMap, &width, &height);

    Tilemap::setSize(width, height);
    Objects::clear();

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            auto tiles = stbte_get_tile(stbTileMap, x, y);
            short tileId = tiles[0]; // Tile layer
            Tilemap::setTile(x, y, tileId < 0 ? Tiles::AIR : Tiles::get(tileId));

            short objectId = tiles[1]; // Object layer
            if (objectId >= OBJECT_ID_OFFSET) {
                int prototypeIndex = objectId - OBJECT_ID_OFFSET;
                auto obj = Objects::instantiateObjectNoInit(prototypeIndex, Vector(x, y));

                float* props = stbte_get_properties(stbTileMap, x, y);
                if (props[0] != 0) {
                    obj->applyTileEditorData(props + 1);
                }

                // initTileEditorData() was already called with the prototype's values,
                // overwrite them with the new values
                obj->getTileEditorProps().clear();
                obj->initTileEditorData(obj->getTileEditorProps());

                obj->postInit();
            }
        }
    }
}
#endif
