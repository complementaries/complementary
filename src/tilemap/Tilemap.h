#ifndef Tilemap_H
#define Tilemap_H

#include "graphics/Buffer.h"
#include "math/Vector.h"
#include "tiles/Tile.h"

namespace Tilemap {
    bool init(int width, int height);

    int getWidth();
    int getHeight();
    void setWidth(int newWidth);
    void setHeight(int newHeight);

    const Tile& getTile(int x, int y);
    void setTile(int x, int y, const Tile& tile);
    Vector getSpawnPoint();

    void render();

    void forceReload();

    void load(const char* path);
    void save(const char* path);

    void reset();
    int getKeys();
}

#endif
