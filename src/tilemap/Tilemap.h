#ifndef Tilemap_H
#define Tilemap_H

#include <vector>

#include "Tile.h"
#include "graphics/gl/Shader.h"
#include "graphics/gl/VertexBuffer.h"

class Tilemap final {
  public:
    Tilemap();
    bool init(int width, int height);

    int getWidth() const;
    int getHeight() const;

    const Tile& getTile(int x, int y) const;
    void setTile(int x, int y, const Tile& tile);

    void render();

  private:
    void prepareRendering();

    GL::Shader shader;
    GL::VertexBuffer buffer;
    bool dirty;

    int width;
    int height;
    std::vector<int> tiles;
};

#endif