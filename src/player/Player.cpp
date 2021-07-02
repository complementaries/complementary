#include <cmath>
#include <iostream>

#include <imgui.h>
#include <imgui/ImGuiUtils.h>

#include "Game.h"
#include "Input.h"
#include "graphics/gl/Shader.h"
#include "graphics/gl/VertexBuffer.h"
#include "math/Vector.h"
#include "objects/Objects.h"
#include "player/Player.h"

static constexpr float step = 0.005f;

static GL::Shader shader;
static GL::VertexBuffer buffer;
static Vector lastPosition;
static Vector position;
static Vector size{0.8f, 0.8f};
static Vector velocity;
static Vector acceleration;
static float moveSpeed = 0.1f;
static float jumpVelocity = 1.5f;
static float gravity = 0.04f;
static Vector drag{0.5f, 0.9f};
static std::array<bool, FACES> collision;

bool Player::init() {
    if (shader.compile({"assets/shaders/player.vs", "assets/shaders/player.fs"})) {
        return true;
    }
    buffer.init(GL::VertexBuffer::Attributes().addVector2());
    return false;
}

bool Player::isColliding(Face face) {
    return collision[face];
}

static bool isColliding(const Tilemap& map) {
    int minX = floorf(position[0]);
    int minY = floorf(position[1]);
    int maxX = floorf(position[0] + size[0]);
    int maxY = floorf(position[1] + size[1]);
    if (minX < 0 || minY < 0 || maxX >= map.getWidth() || maxY >= map.getHeight()) {
        return true;
    }
    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            if (map.getTile(x, y).isSolid()) {
                return true;
            }
        }
    }
    return Objects::collidesWithAny(position, size);
}

void Player::addForce(const Vector& force) {
    acceleration += force;
}

void Player::addForce(Face face, float force) {
    addForce(FaceUtils::getDirection(face) * force);
}

static void tickCollision(const Tilemap& map) {
    for (Face face : FaceUtils::getFaces()) {
        Vector min = position + FaceUtils::getDirection(face) * step;
        Vector max = min + size;

        collision[face] = Objects::handleFaceCollision(min, size, face);

        int minX = floorf(min[0]);
        int minY = floorf(min[1]);
        int maxX = floorf(max[0]);
        int maxY = floorf(max[1]);
        if (minX < 0 || minY < 0 || maxX >= map.getWidth() || maxY >= map.getHeight()) {
            collision[face] = true;
            continue;
        }
        for (int x = minX; x <= maxX; x++) {
            for (int y = minY; y <= maxY; y++) {
                const Tile& tile = map.getTile(x, y);
                if (tile.isSolid()) {
                    collision[face] = true;
                    tile.onFaceCollision(face);
                }
            }
        }
    }

    Vector min = position;
    Vector max = min + size;
    int minX = std::max(static_cast<int>(floorf(min[0])), 0);
    int minY = std::max(static_cast<int>(floorf(min[1])), 0);
    int maxX = std::min(static_cast<int>(floorf(max[0])), map.getWidth() - 1);
    int maxY = std::min(static_cast<int>(floorf(max[1])), map.getHeight() - 1);
    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            map.getTile(x, y).onCollision();
        }
    }

    Objects::handleCollision(position, size);
}

static void move(const Tilemap& map) {
    Vector energy = velocity;
    while (energy[0] != 0.0f || energy[1] != 0.0f) {
        for (int i = 0; i < 2; i++) {
            if (energy[i] == 0.0f) {
                continue;
            }
            float old = position[i];
            if (energy[i] > step) {
                position[i] += step;
                energy[i] -= step;
            } else if (energy[i] < -step) {
                position[i] -= step;
                energy[i] += step;
            } else {
                position[i] += energy[i];
                energy[i] = 0.0f;
            }
            if (isColliding(map)) {
                energy[i] = 0.0f;
                position[i] = old;
                velocity[i] = 0.0f;
            }
        }
    }
}

void Player::kill() {
    position = Vector();
    lastPosition = position;
}

void Player::tick(const Tilemap& map) {
    lastPosition = position;

    acceleration = Vector();
    addForce(Face::RIGHT, Input::getHorizontal() * moveSpeed);
    addForce(Face::DOWN, gravity);

    if (Input::getButton(ButtonType::JUMP).pressed && isColliding(Face::DOWN)) {
        addForce(Face::UP, jumpVelocity);
    }

    velocity += acceleration;
    velocity *= drag;

    move(map);
    tickCollision(map);
}

void Player::render(float lag) {
    shader.use();
    shader.setMatrix("view", Game::viewMatrix);

    Vector i = lastPosition + (position - lastPosition) * lag;

    float data[12] = {i[0], i[1],           i[0] + size[0], i[1],
                      i[0], i[1] + size[1], i[0] + size[0], i[1],
                      i[0], i[1] + size[1], i[0] + size[0], i[1] + size[1]};
    buffer.setData(data, sizeof(data));
    buffer.drawTriangles(6);
}

void Player::renderImGui() {
    ImGui::Begin("Player");

    ImGui::DragFloat("Move Speed", &moveSpeed, 0.02f);
    ImGui::DragFloat("Jump Velocity", &jumpVelocity, 0.1f);
    ImGui::DragFloat("Gravity", &gravity, 0.01f);
    ImGui::DragFloat2("Drag", drag, 0.1f);

    ImGui::Spacing();

    ImGui::DragFloat2("Position", position);
    ImGui::DragFloat2("Size", size);
    ImGui::DragFloat2("Velocity", velocity);

    ImGui::PushDisabled();
    ImGui::DragFloat2("Acceleration", acceleration);
    ImGui::Checkbox("Left", &(collision[Face::LEFT]));
    ImGui::Checkbox("Right", &(collision[Face::RIGHT]));
    ImGui::Checkbox("Up", &(collision[Face::UP]));
    ImGui::Checkbox("Down", &(collision[Face::DOWN]));
    ImGui::PopDisabled();

    ImGui::End();
}
