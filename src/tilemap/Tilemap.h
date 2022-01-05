#ifndef Tilemap_H
#define Tilemap_H

#include "graphics/Buffer.h"
#include "math/Vector.h"
#include "tiles/Tile.h"

namespace Tilemap {
    bool init(int width, int height);

    int getWidth();
    int getHeight();
    void setSize(int newWidth, int newHeight);
    Vector getSize();

    const Tile& getTile(int x, int y);
    void setTile(int x, int y, const Tile& tile);
    Vector getSpawnPoint();

    void renderBackground();
    void render();

    void forceReload();

    bool load(const char* path);
    bool save(const char* path);
}

#endif
