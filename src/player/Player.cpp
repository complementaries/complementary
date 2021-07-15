#include <cmath>
#include <fstream>
#include <iostream>

#include <imgui.h>
#include <imgui/ImGuiUtils.h>

#include "Game.h"
#include "Input.h"
#include "graphics/Buffer.h"
#include "graphics/gl/Shader.h"
#include "graphics/gl/VertexBuffer.h"
#include "math/Vector.h"
#include "objects/Objects.h"
#include "player/Player.h"
#include "tilemap/Tilemap.h"

static constexpr float step = 0.005f;

static GL::Shader shader;
static GL::VertexBuffer buffer;
Vector lastPosition;
Vector position;

struct PlayerData {
    Vector size{0.8f, 0.8f};
    Vector velocity;
    Vector acceleration;
    float moveSpeed = 0.1f;
    float joystickExponent = 5.0f;
    float jumpVelocity = 1.5f;
    float gravity = 0.04f;
    Vector drag{0.5f, 0.9f};
} data;
static Ability abilities[2] = {Ability::NONE, Ability::NONE};
static std::array<bool, FACES> collision;

static bool worldType = false;
static int wallJumpCooldown = 0;

bool Player::init() {
    if (shader.compile({"assets/shaders/player.vs", "assets/shaders/player.fs"})) {
        return true;
    }
    buffer.init(GL::VertexBuffer::Attributes().addVector2().addRGBA());

    load();
    return false;
}

bool Player::isColliding(Face face) {
    return collision[face];
}

static bool isColliding() {
    int minX = floorf(position[0]);
    int minY = floorf(position[1]);
    int maxX = floorf(position[0] + data.size[0]);
    int maxY = floorf(position[1] + data.size[1]);
    if (minX < 0 || minY < 0 || maxX >= Tilemap::getWidth() || maxY >= Tilemap::getHeight()) {
        return true;
    }
    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            if (Tilemap::getTile(x, y).isSolid()) {
                return true;
            }
        }
    }
    return Objects::collidesWithAny(position, data.size);
}

void Player::addForce(const Vector& force) {
    data.acceleration += force;
}

void Player::addForce(Face face, float force) {
    addForce(FaceUtils::getDirection(face) * force);
}

static void tickCollision() {
    for (Face face : FaceUtils::getFaces()) {
        Vector min = position + FaceUtils::getDirection(face) * step;
        Vector max = min + data.size;

        collision[face] = Objects::handleFaceCollision(min, data.size, face);

        int minX = floorf(min[0]);
        int minY = floorf(min[1]);
        int maxX = floorf(max[0]);
        int maxY = floorf(max[1]);
        if (minX < 0 || minY < 0 || maxX >= Tilemap::getWidth() || maxY >= Tilemap::getHeight()) {
            collision[face] = true;
            continue;
        }
        for (int x = minX; x <= maxX; x++) {
            for (int y = minY; y <= maxY; y++) {
                const Tile& tile = Tilemap::getTile(x, y);
                if (tile.isSolid()) {
                    collision[face] = true;
                    tile.onFaceCollision(face);
                }
            }
        }
    }

    Vector min = position;
    Vector max = min + data.size;
    int minX = std::max(static_cast<int>(floorf(min[0])), 0);
    int minY = std::max(static_cast<int>(floorf(min[1])), 0);
    int maxX = std::min(static_cast<int>(floorf(max[0])), Tilemap::getWidth() - 1);
    int maxY = std::min(static_cast<int>(floorf(max[1])), Tilemap::getHeight() - 1);
    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            Tilemap::getTile(x, y).onCollision();
        }
    }

    Objects::handleCollision(position, data.size);
}

static void move() {
    Vector energy = data.velocity;
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
            if (isColliding()) {
                energy[i] = 0.0f;
                position[i] = old;
                data.velocity[i] = 0.0f;
            }
        }
    }
}

void Player::kill() {
    position = Vector();
    lastPosition = position;
}

void Player::setAbilities(Ability dark, Ability light) {
    abilities[0] = dark;
    abilities[1] = light;
}

bool Player::hasAbility(Ability a) {
    return abilities[worldType] == a;
}

bool Player::invertColors() {
    return worldType;
}

void Player::tick() {
    lastPosition = position;

    if (Input::getButton(ButtonType::SWITCH).pressedFirstFrame) {
        worldType = !worldType;
        Tilemap::forceReload();
    }

    data.acceleration = Vector();
    int sign = Input::getHorizontal() < 0 ? -1 : 1;
    addForce(Face::RIGHT,
             powf(std::abs(Input::getHorizontal()), data.joystickExponent) * data.moveSpeed * sign);
    addForce(Face::DOWN, data.gravity);

    if (Input::getButton(ButtonType::JUMP).pressedFirstFrame) {
        if (isColliding(Face::DOWN)) {
            addForce(Face::UP, data.jumpVelocity);
            wallJumpCooldown = 10;
        } else if (hasAbility(Ability::WALL_JUMP) && wallJumpCooldown == 0) {
            if (isColliding(Face::LEFT) && Input::getButton(ButtonType::LEFT).pressed) {
                addForce(Vector(1.5f, -1.0f) * data.jumpVelocity);
            } else if (isColliding(Face::RIGHT) && Input::getButton(ButtonType::RIGHT).pressed) {
                addForce(Vector(-1.5f, -1.0f) * data.jumpVelocity);
            }
        }
    }

    wallJumpCooldown -= wallJumpCooldown > 0;

    data.velocity += data.acceleration;
    data.velocity *= data.drag;

    move();
    tickCollision();
}

void Player::render(float lag) {
    shader.use();
    shader.setMatrix("view", Game::viewMatrix);

    Vector i = lastPosition + (position - lastPosition) * lag;

    Buffer buf;
    float minX = i[0];
    float minY = i[1];
    float maxX = minX + data.size[0];
    float maxY = minY + data.size[1];
    Color color = AbilityUtils::getColor(abilities[worldType]);
    buf.add(minX).add(minY).add(color);
    buf.add(maxX).add(minY).add(color);
    buf.add(minX).add(maxY).add(color);
    buf.add(maxX).add(maxY).add(color);
    buf.add(maxX).add(minY).add(color);
    buf.add(minX).add(maxY).add(color);

    buffer.setData(buf.getData(), buf.getSize());
    buffer.drawTriangles(6);
}

void Player::renderImGui() {
    ImGui::DragFloat("Move Speed", &data.moveSpeed, 0.02f);
    ImGui::DragFloat("Joystick Exponent", &data.joystickExponent, 0.05f);
    ImGui::DragFloat("Jump Velocity", &data.jumpVelocity, 0.1f);
    ImGui::DragFloat("Gravity", &data.gravity, 0.01f);
    ImGui::DragFloat2("Drag", data.drag, 0.1f);

    ImGui::Spacing();

    ImGui::DragFloat2("Position", position);
    ImGui::DragFloat2("Size", data.size);
    ImGui::DragFloat2("Velocity", data.velocity);

    ImGui::PushDisabled();
    ImGui::DragFloat2("Acceleration", data.acceleration);
    ImGui::Checkbox("Left", &(collision[Face::LEFT]));
    ImGui::Checkbox("Right", &(collision[Face::RIGHT]));
    ImGui::Checkbox("Up", &(collision[Face::UP]));
    ImGui::Checkbox("Down", &(collision[Face::DOWN]));
    ImGui::PopDisabled();

    if (ImGui::Button("Load")) {
        load();
    }

    ImGui::SameLine();
    if (ImGui::Button("Save")) {
        printf("hello\n");
        save();
    }
}

void Player::load() {
    std::ifstream stream;
    stream.open("assets/player.cmpl");
    if (!stream.good()) {
        printf("Failed to load player data.\n");
        return;
    }

    char magic[5];
    stream.read(magic, 4);
    magic[4] = 0;

    // File magic must be CMPL
    assert(strcmp(magic, "CMPL") == 0);
    stream.read((char*)&data, sizeof(PlayerData));
}

void Player::save() {
    std::ofstream stream;
    stream.open("assets/player.cmpl");

    stream.write("CMPL", 4);
    stream.write((char*)&data, sizeof(PlayerData));
}
