#ifndef PLAYER_H
#define PLAYER_H

#include "graphics/gl/Shader.h"
#include "graphics/gl/VertexBuffer.h"
#include "tilemap/Tilemap.h"

class Player final {
  public:
    Player();
    bool init();

    void tick(const Tilemap& map);
    void render(float lag);

  private:
    bool isColliding(const Tilemap& map) const;

    GL::Shader shader;
    GL::VertexBuffer buffer;

    float lastX;
    float lastY;
    float x;
    float y;
    float width;
    float height;
    float velocityX;
    float velocityY;
    bool onGround;
};

#endif