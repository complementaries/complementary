#ifndef TILEMAP_EDITOR_H
#define TILEMAP_EDITOR_H

#include "NonCopyable.h"
#include <stb_tilemap_editor.h>

class TilemapEditor final : private NonCopyable {
  public:
    TilemapEditor(int screenWidth, int screenHeight);
    ~TilemapEditor();
    void tick(float dt);
    void render();
    void onMouseEvent(void* eventPointer);
    void onScreenResize(int width, int height);
    void setZoom(float zoom);
    float getZoom();
    void flush();

    static bool init();

  private:
    stbte_tilemap* stbTileMap;
};

#endif
