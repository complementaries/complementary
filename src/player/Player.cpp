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
static Vector lastPosition;
static Vector position;

struct PlayerData {
    Vector size{0.8f, 0.8f};
    Vector velocity;
    Vector acceleration;
    float moveSpeed = 0.04f;
    float joystickExponent = 5.0f;
    float jumpInit = 0.3f;
    float jumpBoost = 0.1f;
    int maxJumpTicks = 40;
    Vector wallJumpInit{0.5f, 0.4f};
    float wallJumpBoost = 0.1f;
    int maxWallJumpTicks = 40;
    float wallJumpDrag = 0.2f;
    int wallJumpMoveCooldown = 15;
    float gravity = 0.03f;
    int coyoteTicks = 13;
    Vector drag{0.7f, 0.9f};
    int maxJumpBufferTicks = 10;
    int maxDashTicks = 24;
    int maxDashCooldown = 24;
    float dashStrength = 0.35f;
};

static PlayerData data;
static Ability abilities[2] = {Ability::NONE, Ability::NONE};
static std::array<bool, FACES> collision;
static int fakeGrounded = 0;
static bool leftWall = false;
static bool rightWall = false;

static bool worldType = false;
static int wallJumpCooldown = 0;
static int jumpTicks = 0;
static int wallJumpTicks = 0;
static Vector wallJumpDirection;
static int leftWallJumpCooldown = 0;
static int rightWallJumpCooldown = 0;
static int jumpBufferTicks = 0;

static int dashTicks = 0;
static int dashCoolDown = 0;
static Vector dashVelocity;
static float dashDirection = 1.0f;
static bool dashUseable = false;

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

void Player::setPosition(const Vector& pos) {
    position = pos;
    lastPosition = pos;
}

static void tickWallJumpCollision(Face face, bool& wall) {
    Vector min = position + FaceUtils::getDirection(face) * step;
    Vector max = min + data.size;

    wall = Objects::hasWallCollision(min, data.size);

    int minX = floorf(min[0]);
    int minY = floorf(min[1]);
    int maxX = floorf(max[0]);
    int maxY = floorf(max[1]);
    if (minX < 0 || minY < 0 || maxX >= Tilemap::getWidth() || maxY >= Tilemap::getHeight()) {
        wall = true;
        return;
    }
    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            const Tile& tile = Tilemap::getTile(x, y);
            if (tile.isSolid() && tile.isWall()) {
                wall = true;
            }
        }
    }
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

    tickWallJumpCollision(Face::LEFT, leftWall);
    tickWallJumpCollision(Face::RIGHT, rightWall);

    Vector min = position;
    Vector max = min + data.size;
    int minX = std::max(static_cast<int>(floorf(min[0])), 0);
    int minY = std::max(static_cast<int>(floorf(min[1])), 0);
    int maxX = std::min(static_cast<int>(floorf(max[0])), Tilemap::getWidth() - 1);
    int maxY = std::min(static_cast<int>(floorf(max[1])), Tilemap::getHeight() - 1);
    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            Tilemap::getTile(x, y).onCollision(x, y);
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
    position = Tilemap::getSpawnPoint();
    lastPosition = position;
    Tilemap::reset();
    setAbilities(Ability::NONE, Ability::NONE);
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

    leftWallJumpCooldown -= leftWallJumpCooldown > 0;
    rightWallJumpCooldown -= rightWallJumpCooldown > 0;

    data.acceleration = Vector();
    int sign = Input::getHorizontal() < 0
                   ? -1 + static_cast<float>(leftWallJumpCooldown) / data.wallJumpMoveCooldown
                   : 1 - static_cast<float>(rightWallJumpCooldown) / data.wallJumpMoveCooldown;
    addForce(Face::RIGHT,
             powf(std::abs(Input::getHorizontal()), data.joystickExponent) * data.moveSpeed * sign);
    addForce(Face::DOWN, data.gravity);

    if (Input::getHorizontal() > 0.0f) {
        dashDirection = 1.0f;
    } else if (Input::getHorizontal() < 0.0f) {
        dashDirection = -1.0f;
    }

    if (Input::getButton(ButtonType::JUMP).pressedFirstFrame) {
        jumpBufferTicks = data.maxJumpBufferTicks;
    }
    jumpBufferTicks -= jumpBufferTicks > 0;

    if (jumpBufferTicks > 0) {
        if (fakeGrounded > 0) {
            addForce(Face::UP, data.jumpInit);
            jumpTicks = data.maxJumpTicks;
            wallJumpCooldown = 10;
            jumpBufferTicks = 0;
        } else if (hasAbility(Ability::WALL_JUMP) && wallJumpCooldown == 0) {
            if (leftWall && Input::getButton(ButtonType::LEFT).pressed) {
                wallJumpDirection = Vector(1.0f, -1.0f);
                addForce(wallJumpDirection * data.wallJumpInit);
                wallJumpTicks = data.maxWallJumpTicks;
                leftWallJumpCooldown = data.wallJumpMoveCooldown;
                jumpBufferTicks = 0;
            } else if (rightWall && Input::getButton(ButtonType::RIGHT).pressed) {
                wallJumpDirection = Vector(-1.0f, -1.0f);
                addForce(wallJumpDirection * data.wallJumpInit);
                wallJumpTicks = data.maxWallJumpTicks;
                rightWallJumpCooldown = data.wallJumpMoveCooldown;
                jumpBufferTicks = 0;
            }
        }
    }
    if (!Input::getButton(ButtonType::JUMP).pressed && jumpTicks > 0) {
        jumpTicks = 0;
    }
    if (jumpTicks > 0) {
        addForce(Face::UP, data.jumpBoost * (1.0f / powf(1.1f, data.maxJumpTicks + 1 - jumpTicks)));
        jumpTicks--;
    }
    if (!Input::getButton(ButtonType::JUMP).pressed && wallJumpTicks > 0) {
        wallJumpTicks = 0;
    }
    if (wallJumpTicks > 0) {
        Vector angle = data.wallJumpInit;
        angle.normalize();
        addForce(wallJumpDirection * angle * data.wallJumpBoost *
                 (1.0f / powf(1.1f, data.maxWallJumpTicks + 1 - wallJumpTicks)));
        wallJumpTicks--;
        dashDirection = wallJumpDirection.x;
    }

    wallJumpCooldown -= wallJumpCooldown > 0;

    data.velocity += data.acceleration;
    Vector actualDrag = data.drag;
    if (((leftWall && Input::getButton(ButtonType::LEFT).pressed) ||
         (rightWall && Input::getButton(ButtonType::RIGHT).pressed)) &&
        hasAbility(Ability::WALL_JUMP) && data.velocity[1] > 0.0f) {
        actualDrag[1] *= data.wallJumpDrag;
    }
    if (hasAbility(Ability::DASH) && Input::getButton(ButtonType::ABILITY).pressedFirstFrame &&
        dashTicks == 0 && dashCoolDown == 0 && dashUseable) {
        dashTicks = data.maxDashTicks;
        dashUseable = false;
        dashCoolDown = data.maxDashCooldown + dashTicks;
        dashVelocity = Vector(data.dashStrength * dashDirection, 0.0f);
    }
    if (leftWall || rightWall) {
        dashUseable = true;
    }
    dashTicks -= dashTicks > 0;
    dashCoolDown -= dashCoolDown > 0;
    if (isColliding(Face::LEFT) || isColliding(Face::RIGHT)) {
        dashCoolDown -= dashTicks;
        dashTicks = 0;
    }
    if (dashTicks > 0) {
        data.velocity =
            dashVelocity * cosf(static_cast<float>(M_PI) * 0.5f *
                                (1.0f - static_cast<float>(dashTicks) / data.maxDashTicks));
    } else {
        data.velocity *= actualDrag;
    }

    move();
    tickCollision();
    if (isColliding(Face::DOWN)) {
        fakeGrounded = data.coyoteTicks;
        dashUseable = true;
    }
    fakeGrounded -= fakeGrounded > 0;

    if (isColliding(Face::UP)) {
        jumpTicks = 0;
        wallJumpTicks = 0;
    }
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
    ImGui::DragFloat("Jump Init", &data.jumpInit, 0.1f);
    ImGui::DragFloat("Jump Boost", &data.jumpBoost, 0.1f);
    ImGui::DragInt("Jump Ticks", &data.maxJumpTicks, 1);
    ImGui::DragFloat2("Wall Jump Init", data.wallJumpInit, 0.1f);
    ImGui::DragFloat("Wall Jump Boost", &data.wallJumpBoost, 0.1f);
    ImGui::DragFloat("Wall Jump Drag", &data.wallJumpDrag, 0.01f);
    ImGui::DragInt("Wall Jump Ticks", &data.maxWallJumpTicks, 1);
    ImGui::DragInt("Wall Jump Move Cooldown", &data.wallJumpMoveCooldown, 1);
    ImGui::DragFloat("Gravity", &data.gravity, 0.01f);
    ImGui::DragFloat2("Drag", data.drag, 0.1f);
    ImGui::DragInt("Coyote Time", &data.coyoteTicks, 1);
    ImGui::DragInt("Jump Buffer Ticks", &data.maxJumpBufferTicks, 1);
    ImGui::DragInt("Dash Ticks", &data.maxDashTicks, 1);
    ImGui::DragInt("Dash Cooldown", &data.maxDashCooldown, 1);
    ImGui::DragFloat("Dash Strength", &data.dashStrength, 0.05f);

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

    if (ImGui::Button("Respawn")) {
        kill();
    }

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
