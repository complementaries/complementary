#include <cmath>
#include <iostream>

#include <imgui.h>
#include <imgui/ImGuiUtils.h>

#include "Game.h"
#include "Input.h"
#include "Player.h"

Player::Player()
    : lastX(0.0f), lastY(0.0f), x(0.0f), y(0.0f), width(0.8f), height(0.8f), velocityX(0.0f),
      velocityY(0.0f), onGround(false) {
}

bool Player::init() {
    if (shader.compile({"assets/shaders/player.vs", "assets/shaders/player.fs"})) {
        return true;
    }
    buffer.init(GL::VertexBuffer::Attributes().addVector2());
    return false;
}

bool Player::isColliding(const Tilemap& map) const {
    int minX = floorf(x);
    int minY = floorf(y);
    int maxX = floorf(x + width);
    int maxY = floorf(y + height);
    if (minX < 0 || minY < 0 || maxX >= map.getWidth() || maxY >= map.getHeight()) {
        return true;
    }
    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            if (map.getTile(x, y).getColor() == 0xFF000000) {
                return true;
            }
        }
    }
    return false;
}

void Player::tick(const Tilemap& map) {
    lastX = x;
    lastY = y;

    velocityX += Input::getHorizontal() * moveSpeed;
    velocityY += gravity;
    if (Input::getButton(ButtonType::JUMP).pressed && onGround) {
        velocityY -= jumpVelocity;
        onGround = false;
    }
    onGround = false;

    velocityX *= dragX;
    velocityY *= dragY;

    const float step = 0.005f;
    float energyX = velocityX;
    float energyY = velocityY;
    while (energyX != 0.0f || energyY != 0.0f) {
        if (energyX != 0.0f) {
            float oldX = x;
            if (energyX > step) {
                x += step;
                energyX -= step;
            } else if (energyX < -step) {
                x -= step;
                energyX += step;
            } else {
                x += energyX;
                energyX = 0.0f;
            }
            if (isColliding(map)) {
                energyX = 0.0f;
                x = oldX;
                velocityX = 0.0f;
            }
        }
        if (energyY != 0.0f) {
            float oldY = y;
            if (energyY > step) {
                y += step;
                energyY -= step;
            } else if (energyY < -step) {
                y -= step;
                energyY += step;
            } else {
                y += energyY;
                energyY = 0.0f;
            }
            if (isColliding(map)) {
                onGround = energyY > 0.0f;
                energyY = 0.0f;
                y = oldY;
                velocityY = 0.0f;
            }
        }
    }
}

void Player::render(float lag) {
    shader.use();
    shader.setMatrix("view", Game::viewMatrix);

    float ix = lastX + (x - lastX) * lag;
    float iy = lastY + (y - lastY) * lag;

    float data[12] = {ix,         iy, ix + width, iy,          ix,         iy + height,
                      ix + width, iy, ix,         iy + height, ix + width, iy + height};
    buffer.setData(data, sizeof(data));
    buffer.drawTriangles(6);
}

void Player::renderImGui() {
    ImGui::Begin("Player");

    ImGui::DragFloat("Move Speed", &moveSpeed, 0.02f);
    ImGui::DragFloat("Jump Velocity", &jumpVelocity, 0.1f);
    ImGui::DragFloat("Gravity", &gravity, 0.01f);
    ImGui::DragFloat2("Drag", &dragX, 0.1f);

    ImGui::Spacing();

    ImGui::DragFloat2("Position", &x);
    ImGui::DragFloat2("Size", &width);
    ImGui::DragFloat2("Velocity", &velocityX);

    ImGui::PushDisabled();
    ImGui::Checkbox("Grounded", &onGround);
    ImGui::PopDisabled();

    ImGui::End();
}
