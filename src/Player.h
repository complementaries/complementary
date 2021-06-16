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
    void renderImGui();

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

    float moveSpeed = 0.1f;
    float jumpVelocity = 1.5f;
    float gravity = 0.04f;
    float dragX = 0.5f;
    float dragY = 0.9f;
};

#endif
