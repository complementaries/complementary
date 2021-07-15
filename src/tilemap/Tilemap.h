#ifndef Tilemap_H
#define Tilemap_H

#include "tiles/Tile.h"

namespace Tilemap {
    bool init(int width, int height);

    int getWidth();
    int getHeight();

    const Tile& getTile(int x, int y);
    void setTile(int x, int y, const Tile& tile);

    void render();

    void forceReload();

    void load(const char* path);
    void save(const char* path);
}

#endif
