#ifndef Tilemap_H
#define Tilemap_H

#include <vector>

#include "graphics/gl/Shader.h"
#include "graphics/gl/VertexBuffer.h"
#include "tiles/Tile.h"

class Tilemap final {
  public:
    Tilemap();
    bool init(int width, int height);

    int getWidth() const;
    int getHeight() const;

    const Tile& getTile(int x, int y) const;
    void setTile(int x, int y, const Tile& tile);

    void render();

    static void setDirty();

  private:
    void prepareRendering();

    GL::Shader shader;
    GL::VertexBuffer buffer;
    int vertices;
    static bool dirty;

    int width;
    int height;
    std::vector<int> tiles;
};

#endif