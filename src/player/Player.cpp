#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>

#include <imgui.h>
#include <imgui/ImGuiUtils.h>

#include "AbilityCutscene.h"
#include "Arguments.h"
#include "Game.h"
#include "GoalCutscene.h"
#include "Input.h"
#include "Savegame.h"
#include "Utils.h"
#include "graphics/Buffer.h"
#include "graphics/RenderState.h"
#include "graphics/gl/Shader.h"
#include "graphics/gl/VertexBuffer.h"
#include "math/Vector.h"
#include "objects/ObjectRenderer.h"
#include "objects/Objects.h"
#include "player/Player.h"
#include "sound/SoundManager.h"
#include "tilemap/Tilemap.h"
#include "tilemap/Tiles.h"

static constexpr float step = 0.0025f;

static GL::Shader shader;
static GL::VertexBuffer buffer;
static Vector lastPosition;
static Vector position;
static Vector lastBaseVelocity;
static Vector baseVelocity;
static float lastRenderForce = 0.0f;
static float renderForce = 0.0f;
static Vector renderOffset;
static Vector lastVelocity;
static std::shared_ptr<ParticleSystem> deathParticles;
static std::shared_ptr<ParticleSystem> walkParticles;
static std::shared_ptr<ParticleSystem> wallStickParticles;
static std::shared_ptr<ParticleSystem> dashParticles;
static std::shared_ptr<ParticleSystem> jumpParticlesLeft;
static std::shared_ptr<ParticleSystem> jumpParticlesRight;
static std::shared_ptr<ParticleSystem> jumpParticles;
static std::shared_ptr<ParticleSystem> walljumpParticlesLeft;
static std::shared_ptr<ParticleSystem> walljumpParticlesRight;
static std::shared_ptr<ParticleSystem> walljumpParticles;
static std::shared_ptr<ParticleSystem> gliderParticles;
static bool useOverrideColor = false;
static Color overrideColor;
static std::shared_ptr<ParticleSystem> colorSwitchParticles;
static std::shared_ptr<ParticleSystem> loseAbilityParticles;

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
    float wallJumpDrag = 0.3f;
    int wallJumpMoveCooldown = 15;
    float gravity = 0.0275f;
    int coyoteTicks = 5;
    Vector drag{0.7f, 0.9f};
    int maxJumpBufferTicks = 6;
    int maxDashTicks = 24;
    int maxDashCooldown = 24;
    float dashStrength = 0.35f;
    float gliderGravity = 0.005f;
    int maxJumpCount = 2;
    int wallJumpBufferTicks = 4;
};

static PlayerData data;
static Ability abilities[2] = {Ability::NONE, Ability::NONE};
static std::array<bool, FACES> collision;
static std::array<bool, FACES> lastCollision;
static int fakeGrounded = 0;
static bool leftWall = false;
static bool rightWall = false;
static int leftWallBuffer = 0;
static int rightWallBuffer = 0;

static bool worldType = false;
static bool allowedToMove = true;
static bool gravityEnabled = true;
static int wallJumpCooldown = 0;
static int jumpTicks = 0;
static int wallJumpTicks = 0;
static Vector wallJumpDirection;
static int leftWallJumpCooldown = 0;
static int rightWallJumpCooldown = 0;
static int jumpBufferTicks = 0;
static int leftWallJumpBuffer = 0;
static int rightWallJumpBuffer = 0;

static int dashTicks = 0;
static int dashCoolDown = 0;
static Vector dashVelocity;
static float dashDirection = 1.0f;
static bool dashUseable = false;

static int jumpCount = 0;

static int idleTicks = 0;
static bool idle = false;

static float lastTopShear = 0.0f;
static float topShear = 0.0f;

static bool stickingToWall = false;

static int dead = 0;
static bool hidden = false;

static float gliderScale = 0.0f;
static int deaths = 0;

bool Player::init() {
    if (shader.compile({"assets/shaders/player.vs", "assets/shaders/player.fs"})) {
        return true;
    }
    buffer.init(GL::VertexBuffer::Attributes().addVector2().addRGBA());

    deathParticles =
        Objects::instantiateObject<ParticleSystem>("assets/particlesystems/death.cmob");
    deathParticles->destroyOnLevelLoad = false;

    walkParticles = Objects::instantiateObject<ParticleSystem>("assets/particlesystems/walk.cmob");
    walkParticles->destroyOnLevelLoad = false;

    wallStickParticles =
        Objects::instantiateObject<ParticleSystem>("assets/particlesystems/wallstick.cmob");
    wallStickParticles->destroyOnLevelLoad = false;

    dashParticles = Objects::instantiateObject<ParticleSystem>("assets/particlesystems/dash.cmob");
    dashParticles->destroyOnLevelLoad = false;

    jumpParticlesLeft =
        Objects::instantiateObject<ParticleSystem>("assets/particlesystems/jumpside.cmob");
    jumpParticlesLeft->destroyOnLevelLoad = false;

    jumpParticlesRight =
        Objects::instantiateObject<ParticleSystem>("assets/particlesystems/jumpside.cmob");
    jumpParticlesRight->destroyOnLevelLoad = false;

    jumpParticles = Objects::instantiateObject<ParticleSystem>("assets/particlesystems/jump.cmob");
    jumpParticles->destroyOnLevelLoad = false;

    walljumpParticlesLeft =
        Objects::instantiateObject<ParticleSystem>("assets/particlesystems/walljumpside.cmob");
    walljumpParticlesLeft->destroyOnLevelLoad = false;

    walljumpParticlesRight =
        Objects::instantiateObject<ParticleSystem>("assets/particlesystems/walljumpside.cmob");
    walljumpParticlesRight->destroyOnLevelLoad = false;

    walljumpParticles =
        Objects::instantiateObject<ParticleSystem>("assets/particlesystems/walljump.cmob");
    walljumpParticles->destroyOnLevelLoad = false;

    colorSwitchParticles =
        Objects::instantiateObject<ParticleSystem>("assets/particlesystems/switch.cmob");
    colorSwitchParticles->destroyOnLevelLoad = false;

    loseAbilityParticles =
        Objects::instantiateObject<ParticleSystem>("assets/particlesystems/loseability.cmob");
    loseAbilityParticles->destroyOnLevelLoad = false;

    gliderParticles =
        Objects::instantiateObject<ParticleSystem>("assets/particlesystems/glider.cmob");
    gliderParticles->destroyOnLevelLoad = false;

    return false;
}

static bool wasColliding(Face face) {
    return lastCollision[static_cast<int>(face)];
}

bool Player::isColliding(Face face) {
    return collision[static_cast<int>(face)];
}

bool Player::isColliding(const ObjectBase& o) {
    return o.collidesWith(position, data.size);
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
    return Objects::collidesWithAnySolid(position, data.size);
}

bool Player::isCollidingWithAnyObject() {
    return Objects::collidesWithAny(position, data.size);
}

bool Player::isCollidingInAnyWorld() {
    return Objects::collidesWithSolidInAnyWorld(position, data.size);
}

void Player::addForce(const Vector& force) {
    data.acceleration += force;
}

void Player::addForce(Face face, float force) {
    addForce(FaceUtils::getDirection(face) * force);
}

static void setRenderForceFace(Face face) {
    renderOffset = (FaceUtils::getDirection(face) + Vector(1.0f, 1.0f)) * 0.5f;
}

static void addRenderForce(float force, Face face) {
    renderForce += force;
    setRenderForceFace(face);
}

void Player::addBaseVelocity(const Vector& v) {
    baseVelocity += v;
}

void Player::moveForced(const Vector& v) {
    position += v;
}

Vector Player::getSize() {
    return data.size;
}

Vector Player::getPosition() {
    return position;
}

void Player::setPosition(const Vector& pos) {
    position = pos;
    lastPosition = pos;
}

Vector Player::getCenter(float lag) {
    return lastPosition + (position - lastPosition) * lag + data.size * 0.5f;
}

Vector Player::getVelocity() {
    return data.velocity;
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
    lastCollision = collision;

    for (Face face : FaceUtils::getFaces()) {
        Vector min = position + FaceUtils::getDirection(face) * step;
        if (face == Face::DOWN) {
            float base = lastBaseVelocity.y;
            if (base < 0.0) {
                base = 0.0f;
            }
            min += FaceUtils::getDirection(face) * base;
        }
        Vector max = min + data.size;

        int faceAsInt = static_cast<int>(face);
        collision[faceAsInt] = Objects::handleFaceCollision(min, data.size, face);

        int minX = floorf(min[0]);
        int minY = floorf(min[1]);
        int maxX = floorf(max[0]);
        int maxY = floorf(max[1]);
        if (minX < 0 || minY < 0 || maxX >= Tilemap::getWidth() || maxY >= Tilemap::getHeight()) {
            collision[faceAsInt] = true;
            continue;
        }
        for (int x = minX; x <= maxX; x++) {
            for (int y = minY; y <= maxY; y++) {
                const Tile& tile = Tilemap::getTile(x, y);
                if (tile.isSolid()) {
                    collision[faceAsInt] = true;
                    tile.onFaceCollision(face);
                }
            }
        }
    }

    tickWallJumpCollision(Face::LEFT, leftWall);
    if (leftWall) {
        leftWallBuffer = 5;
    }
    tickWallJumpCollision(Face::RIGHT, rightWall);
    if (rightWall) {
        rightWallBuffer = 5;
    }

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

static void onKill() {
    Player::restart();
    Game::fadeIn(4);
}

void Player::restart() {
    position = Tilemap::getSpawnPoint();
    lastPosition = position;
    Objects::reset();
    Player::setAbilities(Ability::NONE, Ability::NONE, false);
    baseVelocity = Vector();
    lastRenderForce = 0.0f;
    renderForce = 0.0f;
    renderOffset = Vector();
    lastVelocity = Vector();
    data.velocity = Vector();
    data.acceleration = Vector();
    fakeGrounded = 0;
    leftWall = false;
    rightWall = false;
    leftWallBuffer = 0;
    rightWallBuffer = 0;
    worldType = false;
    wallJumpCooldown = 0;
    jumpTicks = 0;
    wallJumpTicks = 0;
    wallJumpDirection = Vector();
    leftWallJumpCooldown = 0;
    rightWallJumpCooldown = 0;
    jumpBufferTicks = 0;
    leftWallJumpBuffer = 0;
    rightWallJumpBuffer = 0;
    dashTicks = 0;
    dashCoolDown = 0;
    dashVelocity = Vector();
    dashDirection = 1.0f;
    dashUseable = false;
    jumpCount = 0;
    idleTicks = 0;
    idle = false;
    lastTopShear = 0.0f;
    topShear = 0.0f;
    stickingToWall = false;
    dead = 0;
    Tilemap::forceReload();
    Game::resetTickCounter();
    if (SoundManager::getMusicChannel() == SoundManager::darkSoundID) {
        SoundManager::switchMusic();
    }
    Game::setBackgroundParticleColor();
    ObjectRenderer::clearStaticBuffer();
    deaths++;
}

void Player::kill() {
    if (GoalCutscene::isActive() || isDead()) {
        return;
    }
    dashParticles->stop();
    SoundManager::playSoundEffect(Sound::DEATH);
    deathParticles->position = getCenter();
    deathParticles->play();
    dead = deathParticles->data.duration + deathParticles->data.maxLifetime;
    RenderState::addRandomizedShake(1.0f);
    Game::fadeOut(4);
    Input::playRumble(1.0f, 200);
}

bool Player::isDead() {
    return dead > 0;
}

bool Player::isAllowedToMove() {
    return allowedToMove && !Game::isFading();
}

void Player::setAllowedToMove(bool value) {
    allowedToMove = value;
}

void Player::setGravityEnabled(bool value) {
    gravityEnabled = value;
}

void Player::resetVelocity() {
    data.velocity = Vector();
    baseVelocity = Vector();
}

void Player::resetDash() {
    dashTicks = 0;
    dashCoolDown = 0;
    dashUseable = true;
}

void Player::setOverrideColor(Color color) {
    overrideColor = color;
    useOverrideColor = true;
}

Color Player::getOverrideColor() {
    return overrideColor;
}

void Player::resetOverrideColor() {
    useOverrideColor = false;
}

void Player::setHidden(bool isHidden) {
    hidden = isHidden;
}

void Player::setAbilities(Ability dark, Ability light, bool animate) {
    Ability lastAbility = Player::getAbility();

    if (dark != Ability::NONE && light != Ability::NONE) {
        Input::playRumble(0.05f, 100);
        if (!hasAbility(dark) && !hasAbility(light)) {
            abilities[0] = dark;
            abilities[1] = light;
            if (!Savegame::abilitiesUnlocked(dark, light) && !Game::inTitleScreen()) {
                Savegame::unlockAbilities(dark, light);
                Savegame::save();
                if (!Arguments::skipAnim) {
                    AbilityCutscene::show(lastAbility);
                }
            }
            PlayerParticles::setParticleColor(colorSwitchParticles);
            if (animate) {
                colorSwitchParticles->play();
            }
            PlayerParticles::setParticlePosition(colorSwitchParticles, 0, 0, 0, 0);
        }

    } else if (!hasAbility(Ability::NONE)) {
        PlayerParticles::setParticlePosition(loseAbilityParticles, 0, 0, 0, 0);
        PlayerParticles::setParticleColor(loseAbilityParticles);
        if (animate) {
            loseAbilityParticles->play();
        }
        abilities[0] = dark;
        abilities[1] = light;
    }

    PlayerParticles::setParticleColors();
}

bool Player::hasAbility(Ability a) {
    return abilities[worldType] == a;
}

Ability Player::getAbility() {
    return abilities[worldType];
}

Ability Player::getPassiveAbility() {
    return abilities[!worldType];
}

bool Player::isDashing() {
    return dashTicks > 0;
}

bool Player::isGliding() {
    return hasAbility(Ability::GLIDER) &&
           (Input::getButton(ButtonType::ABILITY).pressed ||
            Input::getButton(ButtonType::SWITCH_AND_ABILITY).pressed) &&
           isAllowedToMove();
}

bool Player::isGrounded() {
    return fakeGrounded > 0;
}

bool Player::isWallSticking() {
    return hasAbility(Ability::WALL_JUMP) && !isGrounded() &&
           (leftWallBuffer > 0 || rightWallBuffer > 0);
}

bool Player::invertColors() {
    return worldType;
}

void Player::toggleWorld() {
    worldType = !worldType;
    Tilemap::forceReload();
    if (!hasAbility(Ability::NONE)) {
        PlayerParticles::setParticleColors();
        PlayerParticles::setParticlePosition(colorSwitchParticles, 0, 0, 0, 0);
        colorSwitchParticles->play();
    }
}

static void tickIdleAndRunAnimation() {
    if (!Player::isColliding(Face::DOWN)) {
        idle = false;
        idleTicks = 0;
        return;
    }
    if (std::abs(renderForce) < 0.05f) {
        idle = true;
    }
    if (!idle) {
        return;
    }
    idle = true;
    idleTicks++;
    float base = data.velocity.x - lastBaseVelocity.x;
    if (std::abs(base) < 0.01f) {
        addRenderForce(sinf(idleTicks * 0.08f) * 0.01f, Face::DOWN);
    }
}

static void addTopShear(float shear) {
    topShear += shear;
    topShear = std::clamp(topShear, -0.3f, 0.3f);
}

static void tickShear() {
    lastTopShear = topShear;
    if (Player::isColliding(Face::DOWN)) {
        float base = data.velocity.x;
        base -= lastBaseVelocity.x;
        addTopShear(base * 0.2f);
    }
    topShear *= 0.9f;
}

void Player::tick() {
    if (dead > 0) {
        dead--;
        if (dead == 0) {
            onKill();
        }
        return;
    }
    lastVelocity = data.velocity;
    lastRenderForce = renderForce;
    lastPosition = position;

    leftWallJumpCooldown -= leftWallJumpCooldown > 0;
    rightWallJumpCooldown -= rightWallJumpCooldown > 0;

    if (isAllowedToMove()) {
        int sign = Input::getHorizontal() < 0
                       ? -1 + static_cast<float>(leftWallJumpCooldown) / data.wallJumpMoveCooldown
                       : 1 - static_cast<float>(rightWallJumpCooldown) / data.wallJumpMoveCooldown;
        addForce(Face::RIGHT, powf(std::abs(Input::getHorizontal()), data.joystickExponent) *
                                  data.moveSpeed * sign);
    }

    if (gravityEnabled) {
        if (hasAbility(Ability::GLIDER) && data.velocity.y > 0 &&
            (Input::getButton(ButtonType::ABILITY).pressed ||
             Input::getButton(ButtonType::SWITCH_AND_ABILITY).pressed) &&
            isAllowedToMove()) {
            addForce(Face::DOWN, data.gliderGravity);
            gliderParticles->data.boxSize.x = 1.5f;
            PlayerParticles::setParticlePosition(
                gliderParticles, -1, -1, -(data.size.x - gliderParticles->data.boxSize.x) / 2.0f,
                -0.25f);
            gliderParticles->play();
        } else {
            addForce(Face::DOWN, data.gravity);
            gliderParticles->stop();
        }
    }

    if (Input::getHorizontal() > 0.0f) {
        dashDirection = 1.0f;
    } else if (Input::getHorizontal() < 0.0f) {
        dashDirection = -1.0f;
    }

    if (hasAbility(Ability::DASH) &&
        (Input::getButton(ButtonType::ABILITY).pressedFirstFrame ||
         Input::getButton(ButtonType::SWITCH_AND_ABILITY).pressedFirstFrame) &&
        dashTicks == 0 && dashCoolDown == 0 && dashUseable && isAllowedToMove()) {
        dashParticles->play();
        Input::playRumble(0.2f, 100);
        dashTicks = data.maxDashTicks;
        dashUseable = false;
        dashCoolDown = data.maxDashCooldown + dashTicks;
        dashVelocity = Vector(data.dashStrength * dashDirection, 0.0f);
        addRenderForce(-0.5f, dashDirection < 0.0f ? Face::LEFT : Face::RIGHT);
        SoundManager::playSoundEffect(Sound::DASH);
        RenderState::addRandomizedShake(0.1f);
    }

    if (Input::getButton(ButtonType::JUMP).pressedFirstFrame && isAllowedToMove()) {
        jumpBufferTicks = data.maxJumpBufferTicks;
    }
    jumpBufferTicks -= jumpBufferTicks > 0;

    if (leftWallBuffer > 0 && Input::getButton(ButtonType::LEFT).pressed && isAllowedToMove()) {
        leftWallJumpBuffer = data.wallJumpBufferTicks;
    } else if (rightWallBuffer > 0 && Input::getButton(ButtonType::RIGHT).pressed &&
               isAllowedToMove()) {
        rightWallJumpBuffer = data.wallJumpBufferTicks;
    }
    if (jumpBufferTicks > 0) {
        if ((fakeGrounded > 0 ||
             (hasAbility(Ability::DOUBLE_JUMP) && jumpCount < data.maxJumpCount)) &&
            dashTicks == 0) {
            addForce(Face::UP, data.jumpInit);
            jumpTicks = data.maxJumpTicks;
            wallJumpCooldown = 10;
            jumpBufferTicks = 0;
            addRenderForce(1.0f, Face::UP);
            jumpCount += 1 + (fakeGrounded <= 0);
            data.velocity.y = 0;
            fakeGrounded = 0;
            SoundManager::playSoundEffect(Sound::JUMP);
            addTopShear(-data.velocity.x * 12.f);
            // TODO: Find out why multiply with 0.9f breaks these particle velocities
            PlayerParticles::setParticlePosition(jumpParticlesLeft, -1, 1, 0,
                                                 -jumpParticlesLeft->data.startSize / 2.0f);
            PlayerParticles::setParticlePosition(jumpParticlesRight, 1, 1, 0,
                                                 -jumpParticlesRight->data.startSize / 2.0f);
            // -------------------------------------------------------------------------------
            PlayerParticles::setParticlePosition(jumpParticles, -1, 1, 0.5,
                                                 -jumpParticles->getColliderOffset());
            PlayerParticles::setParticleVelocities(jumpParticlesLeft, -1, -1, -1, -1);
            PlayerParticles::setParticleVelocities(jumpParticlesRight, 1, 1, -1, -1);
            jumpParticlesLeft->play();
            jumpParticlesRight->play();
            jumpParticles->play();
        } else if (hasAbility(Ability::WALL_JUMP) && wallJumpCooldown == 0) {
            if (leftWallJumpBuffer > 0) {
                wallJumpDirection = Vector(1.0f, -1.0f);
                addForce(wallJumpDirection * data.wallJumpInit);
                wallJumpTicks = data.maxWallJumpTicks;
                leftWallJumpCooldown = data.wallJumpMoveCooldown;
                jumpBufferTicks = 0;
                addRenderForce(-0.5f, Face::LEFT);
                SoundManager::playSoundEffect(Sound::JUMP);
                addTopShear(-0.5f);
                leftWallJumpBuffer = 0;
                rightWallJumpBuffer = 0;
                leftWallBuffer = 0;
                rightWallBuffer = 0;

                PlayerParticles::setParticlePosition(walljumpParticlesLeft, -1, -1,
                                                     walljumpParticlesLeft->getColliderOffset(), 0);
                PlayerParticles::setParticlePosition(
                    walljumpParticlesRight, -1, 1, walljumpParticlesRight->getColliderOffset(), 0);
                PlayerParticles::setParticlePosition(walljumpParticles, -1, -1,
                                                     walljumpParticles->getColliderOffset(), 0.5);
                PlayerParticles::setParticleVelocities(walljumpParticlesLeft, 1, 1, -1, -1);
                PlayerParticles::setParticleVelocities(walljumpParticlesRight, 1, 1, 1, 1);
                PlayerParticles::setParticleVelocities(walljumpParticles, 1, 1, -1, -1);
                walljumpParticlesLeft->play();
                walljumpParticlesRight->play();
                walljumpParticles->play();

            } else if (rightWallJumpBuffer > 0) {
                wallJumpDirection = Vector(-1.0f, -1.0f);
                addForce(wallJumpDirection * data.wallJumpInit);
                wallJumpTicks = data.maxWallJumpTicks;
                rightWallJumpCooldown = data.wallJumpMoveCooldown;
                jumpBufferTicks = 0;
                addRenderForce(-0.5f, Face::RIGHT);
                SoundManager::playSoundEffect(Sound::JUMP);
                addTopShear(0.5f);
                leftWallJumpBuffer = 0;
                rightWallJumpBuffer = 0;
                leftWallBuffer = 0;
                rightWallBuffer = 0;

                PlayerParticles::setParticlePosition(
                    walljumpParticlesLeft, 1, 1, -walljumpParticlesLeft->getColliderOffset(), 0);
                PlayerParticles::setParticlePosition(
                    walljumpParticlesRight, 1, -1, -walljumpParticlesRight->getColliderOffset(), 0);
                PlayerParticles::setParticlePosition(walljumpParticles, 1, -1,
                                                     -walljumpParticles->getColliderOffset(), 0.5);
                PlayerParticles::setParticleVelocities(walljumpParticlesLeft, -1, -1, 1, 1);
                PlayerParticles::setParticleVelocities(walljumpParticlesRight, -1, -1, -1, -1);
                PlayerParticles::setParticleVelocities(walljumpParticles, -1, -1, -1, -1);
                walljumpParticlesLeft->play();
                walljumpParticlesRight->play();
                walljumpParticles->play();
            }
        }
    }
    leftWallBuffer -= leftWallBuffer > 0;
    rightWallBuffer -= rightWallBuffer > 0;
    leftWallJumpBuffer -= leftWallJumpBuffer > 0;
    rightWallJumpBuffer -= rightWallJumpBuffer > 0;
    if (!Input::getButton(ButtonType::JUMP).pressed && jumpTicks > 0 && isAllowedToMove()) {
        jumpTicks = 0;
    }
    if (jumpTicks > 0) {
        addForce(Face::UP, data.jumpBoost * (1.0f / powf(1.1f, data.maxJumpTicks + 1 - jumpTicks)));
        jumpTicks--;
    }
    if (!Input::getButton(ButtonType::JUMP).pressed && wallJumpTicks > 0 && isAllowedToMove()) {
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
    data.acceleration = Vector();
    Vector actualDrag = data.drag;
    if (hasAbility(Ability::WALL_JUMP) && data.velocity[1] > 0.0f) {
        int xLeft = position.x - data.size.x * 0.5f;
        int xRight = position.x + data.size.x * 1.5f;
        int y = position.y + data.size.y;
        xLeft = std::clamp(xLeft, 0, Tilemap::getWidth() - 1);
        xRight = std::clamp(xRight, 0, Tilemap::getWidth() - 1);
        y = std::clamp(y, 0, Tilemap::getHeight() - 1);
        if (leftWall && Input::getButton(ButtonType::LEFT).pressed && isAllowedToMove()) {
            actualDrag[1] *= data.wallJumpDrag;
            setRenderForceFace(Face::LEFT);
            resetDash();

            PlayerParticles::setParticlePosition(wallStickParticles, -1, 1,
                                                 wallStickParticles->getColliderOffset(),
                                                 wallStickParticles->getColliderOffset());
            PlayerParticles::setParticleVelocities(wallStickParticles, 1, 1, 1, 1);
            if (!stickingToWall) {
                wallStickParticles->play();
                stickingToWall = true;
            }
            if (Tilemap::getTile(xLeft, y).getId() <= 0 &&
                !Objects::collidesWithAnySolid(Vector(xLeft, y), Vector(0.1f, 0.1f))) {
                wallStickParticles->stop();
            }
        } else if (rightWall && Input::getButton(ButtonType::RIGHT).pressed && isAllowedToMove()) {
            actualDrag[1] *= data.wallJumpDrag;
            setRenderForceFace(Face::RIGHT);
            resetDash();

            PlayerParticles::setParticlePosition(wallStickParticles, 1, 1,
                                                 -wallStickParticles->getColliderOffset(),
                                                 wallStickParticles->getColliderOffset());
            PlayerParticles::setParticleVelocities(wallStickParticles, -1, -1, 1, 1);
            if (!stickingToWall) {
                wallStickParticles->play();
                stickingToWall = true;
            }
            if (Tilemap::getTile(xRight, y).getId() <= 0 &&
                !Objects::collidesWithAnySolid(Vector(xRight, y), Vector(0.1f, 0.1f))) {
                wallStickParticles->stop();
            }
        } else {
            wallStickParticles->stop();
            stickingToWall = false;
        }
    } else {
        wallStickParticles->stop();
        stickingToWall = false;
    }
    if ((leftWall || rightWall) && Player::hasAbility(Ability::WALL_JUMP)) {
        resetDash();
    }
    dashTicks -= dashTicks > 0;
    dashCoolDown -= dashCoolDown > 0;
    if (isColliding(Face::LEFT) || isColliding(Face::RIGHT)) {
        dashParticles->stop();
    }
    if (dashTicks > 0) {
        data.velocity =
            dashVelocity * cosf(static_cast<float>(M_PI) * 0.5f *
                                (1.0f - static_cast<float>(dashTicks) / data.maxDashTicks));
        if (data.velocity.x > 0) {
            PlayerParticles::setParticlePosition(dashParticles, -1, -1, 0, 0.5);
            PlayerParticles::setParticleVelocities(dashParticles, 1, 1, 1, 1);
        } else {
            PlayerParticles::setParticlePosition(dashParticles, 1, -1, 0, 0.5);
            PlayerParticles::setParticleVelocities(dashParticles, -1, -1, 1, 1);
        }
    } else {
        data.velocity *= actualDrag;
        data.velocity += (Vector(1.0f, 1.0f) - actualDrag) * baseVelocity;
        lastBaseVelocity = baseVelocity;
        baseVelocity = Vector();
        dashParticles->stop();
    }

    float fallStrenght = data.velocity.y;

    move();
    tickCollision();

    if (isColliding(Face::DOWN)) {
        fakeGrounded = data.coyoteTicks;
        dashUseable = true;
        float velX = data.velocity.x - lastBaseVelocity.x;
        if (std::abs(velX) > 0.02f && dashTicks <= 0) {
            if (velX > 0) {
                PlayerParticles::setParticleVelocities(walkParticles, -1, 1, -1, -1);
                PlayerParticles::setParticlePosition(walkParticles, -1, 1,
                                                     walkParticles->data.startSize * 2.0f, 0);
            } else {
                PlayerParticles::setParticleVelocities(walkParticles, 1, -1, -1, -1);
                PlayerParticles::setParticlePosition(walkParticles, 1, 1,
                                                     -walkParticles->data.startSize * 2.0f, 0);
            }

            walkParticles->play();
        } else {
            walkParticles->stop();
        }
    } else {
        walkParticles->stop();
    }
    fakeGrounded -= fakeGrounded > 0;

    if (fakeGrounded > 0) {
        jumpCount = 0;
    }

    if (isColliding(Face::UP)) {
        jumpTicks = 0;
        wallJumpTicks = 0;
    }

    tickIdleAndRunAnimation();
    tickShear();

    if (!wasColliding(Face::DOWN) && isColliding(Face::DOWN)) {
        addRenderForce(-2.0f * fallStrenght, Face::DOWN);
    }
    if (!wasColliding(Face::UP) && isColliding(Face::UP)) {
        addRenderForce(2.0f * fallStrenght, Face::UP);
    }
    if (!wasColliding(Face::RIGHT) && isColliding(Face::RIGHT) && lastVelocity.x > 0.0f) {
        addRenderForce(0.25f, Face::RIGHT);
    }
    if (!wasColliding(Face::LEFT) && isColliding(Face::LEFT) && lastVelocity.x < 0.0f) {
        addRenderForce(0.25f, Face::LEFT);
    }
    renderForce *= 0.95f;

    if (isGliding() && !isColliding(Face::DOWN)) {
        gliderScale += 0.04f;
    } else {
        gliderScale -= 0.02f;
    }
    gliderScale = std::clamp(gliderScale, 0.0f, 1.0f);
}

static void addGlider(Buffer& buf, Color color) {
    float oy = -0.25f + 0.75f * gliderScale;
    float gliderWidth = 1.5f * gliderScale;
    float gliderLeft = 0.5f - 0.75f * gliderScale;
    float gliderRight = gliderLeft + gliderWidth;
    float dia = 0.5f * gliderScale;
    float thickness = 0.3f * gliderScale;

    buf.add(gliderLeft).add(-oy).add(color);
    buf.add(gliderRight).add(-oy).add(color);
    buf.add(gliderLeft).add(-oy + thickness).add(color);

    buf.add(gliderRight).add(-oy).add(color);
    buf.add(gliderLeft).add(-oy + thickness).add(color);
    buf.add(gliderRight).add(-oy + thickness).add(color);

    buf.add(gliderLeft).add(-oy).add(color);
    buf.add(gliderLeft).add(-oy + thickness).add(color);
    buf.add(gliderLeft - dia).add(-oy + dia).add(color);

    buf.add(gliderLeft).add(-oy + thickness).add(color);
    buf.add(gliderLeft - dia).add(-oy + dia).add(color);
    buf.add(gliderLeft - dia).add(-oy + dia + thickness).add(color);

    buf.add(gliderRight).add(-oy).add(color);
    buf.add(gliderRight).add(-oy + thickness).add(color);
    buf.add(gliderRight + dia).add(-oy + dia).add(color);

    buf.add(gliderRight).add(-oy + thickness).add(color);
    buf.add(gliderRight + dia).add(-oy + dia).add(color);
    buf.add(gliderRight + dia).add(-oy + dia + thickness).add(color);
}

void Player::render(float lag) {
    if (dead > 0 || hidden) {
        return;
    }
    shader.use();
    RenderState::setViewMatrix(shader);

    Matrix model;
    model.transform(lastPosition + (position - lastPosition) * lag);
    model.scale(data.size);
    model.scale(Vector(1.005f, 1.005f));
    constexpr float maxWobble = 1.5f;
    float wobble = 1.0f + (lastRenderForce + (renderForce - lastRenderForce) * lag);
    if (wobble > maxWobble) {
        wobble = maxWobble;
    }
    if (wobble < 1.0f / maxWobble) {
        wobble = 1.0f / maxWobble;
    }
    model.transform(renderOffset);
    model.scale(Vector(1.0f / wobble, wobble));
    model.transform(-renderOffset);
    shader.setMatrix("model", model);

    static Buffer buf;
    buf.clear();
    Color color = useOverrideColor ? overrideColor : AbilityUtils::getColor(abilities[worldType]);

    float base = lastBaseVelocity.y * 2.0f;
    if (base < 0.0) {
        base = 0.0f;
    }

    float shear = lastTopShear + (topShear - lastTopShear) * lag;
    buf.add(0.0f).add(0.0f).add(color);
    buf.add(1.0f).add(0.0f).add(color);
    buf.add(shear + 0.0f).add(1.0f + base).add(color);
    buf.add(shear + 1.0f).add(1.0f + base).add(color);
    buf.add(1.0f).add(0.0f).add(color);
    buf.add(shear + 0.0f).add(1.0f + base).add(color);
    addGlider(buf, color);

    buffer.setStreamData(buf.getData(), buf.getSize());
    buffer.drawTriangles(buf.getSize() / (sizeof(float) * 2 + sizeof(Color)));
}

void Player::renderImGui() {
    ImGui::Indent();

    if (ImGui::Button("Switch ability")) {
        Ability ability = abilities[worldType];
        ability = static_cast<Ability>(static_cast<int>(ability) + 1);
        if (ability >= Ability::MAX) {
            ability = static_cast<Ability>(0);
        }
        abilities[worldType] = static_cast<Ability>(ability);
    }

    if (ImGui::CollapsingHeader("Movement")) {
        ImGui::DragFloat2("Position", position);
        ImGui::DragFloat2("Size", data.size);
        ImGui::DragFloat2("Velocity", data.velocity);
        ImGui::DragFloat2("Acceleration", data.acceleration);
        ImGui::DragFloat("last top", &lastTopShear);
        ImGui::DragFloat("top", &topShear);

        ImGui::Spacing();

        ImGui::DragFloat("Move Speed", &data.moveSpeed, 0.02f);
        ImGui::DragFloat("Joystick Exponent", &data.joystickExponent, 0.05f);
        ImGui::DragFloat("Gravity", &data.gravity, 0.01f);
        ImGui::DragFloat2("Drag", data.drag, 0.1f);
    }

    if (ImGui::CollapsingHeader("Jump")) {
        ImGui::Indent();
        ImGui::DragFloat("Jump Init", &data.jumpInit, 0.1f);
        ImGui::DragFloat("Jump Boost", &data.jumpBoost, 0.1f);
        ImGui::DragInt("Jump Ticks", &data.maxJumpTicks, 1);
        ImGui::DragInt("Coyote Time", &data.coyoteTicks, 1);
        ImGui::DragInt("Jump Buffer Ticks", &data.maxJumpBufferTicks, 1);
        ImGui::Unindent();
    }

    if (ImGui::CollapsingHeader("Wall Jump")) {
        ImGui::Indent();
        ImGui::DragFloat2("Init", data.wallJumpInit, 0.1f);
        ImGui::DragFloat("Boost", &data.wallJumpBoost, 0.1f);
        ImGui::DragFloat("Drag", &data.wallJumpDrag, 0.01f);
        ImGui::DragInt("Ticks", &data.maxWallJumpTicks, 1);
        ImGui::DragInt("Move Cooldown", &data.wallJumpMoveCooldown, 1);
        ImGui::Unindent();
    }

    if (ImGui::CollapsingHeader("Dash")) {
        ImGui::Indent();
        ImGui::DragInt("Dash Ticks", &data.maxDashTicks, 1);
        ImGui::DragInt("Dash Cooldown", &data.maxDashCooldown, 1);
        ImGui::DragFloat("Dash Strength", &data.dashStrength, 0.05f);
        ImGui::Unindent();
    }

    if (ImGui::CollapsingHeader("Glider")) {
        ImGui::DragFloat("Glider Gravity", &data.gliderGravity, 0.01f);
    }

    if (ImGui::CollapsingHeader("Double Jump")) {
        ImGui::DragInt("Maximum Jump Count", &data.maxJumpCount, 1);
    }

    ImGui::Spacing();

    ImGui::PushDisabled();
    ImGui::Checkbox("Left", &(collision[static_cast<int>(Face::LEFT)]));
    ImGui::Checkbox("Right", &(collision[static_cast<int>(Face::RIGHT)]));
    ImGui::Checkbox("Up", &(collision[static_cast<int>(Face::UP)]));
    ImGui::Checkbox("Down", &(collision[static_cast<int>(Face::DOWN)]));
    ImGui::PopDisabled();

    if (ImGui::Button("Respawn")) {
        kill();
    }
    ImGui::Unindent();
}

// PARTICLE HELPERS

void PlayerParticles::setParticleVelocities(std::shared_ptr<ParticleSystem> particles, int xMinSign,
                                            int xMaxSign, int yMinMSign, int yMaxSign) {
    Vector minVelocity = particles->data.minStartVelocity;
    Vector maxVelocity = particles->data.maxStartVelocity;

    particles->data.minStartVelocity =
        Vector(std::abs(minVelocity.x) * xMinSign, std::abs(minVelocity.y) * yMinMSign);
    particles->data.maxStartVelocity =
        Vector(std::abs(maxVelocity.x) * xMaxSign, std::abs(maxVelocity.y) * yMaxSign);
}

void PlayerParticles::setParticlePosition(std::shared_ptr<ParticleSystem> particles, int xCoord,
                                          int yCoord, float xOffset, float yOffset) {
    particles->position = Player::getCenter() + Vector((xCoord * data.size.x) / 2.0f + xOffset,
                                                       (yCoord * data.size.y) / 2.0f + yOffset);
}

void PlayerParticles::setParticleColor(std::shared_ptr<ParticleSystem> particles) {
    Color color = AbilityUtils::getColor(abilities[worldType]);

    particles->data.startColor = color;
    color = ColorUtils::setAlpha(color, 0);
    particles->data.endColor = color;
}

void PlayerParticles::setParticleColors() {
    setParticleColor(deathParticles);
    setParticleColor(walkParticles);
    setParticleColor(wallStickParticles);
    setParticleColor(dashParticles);
    setParticleColor(jumpParticlesLeft);
    setParticleColor(jumpParticlesRight);
    setParticleColor(jumpParticles);
    setParticleColor(walljumpParticlesLeft);
    setParticleColor(walljumpParticlesRight);
    setParticleColor(walljumpParticles);
    setParticleColor(colorSwitchParticles);
    setParticleColor(gliderParticles);
}

int Player::getDeaths() {
    return deaths;
}

void Player::resetDeaths() {
    deaths = 0;
}

void Player::subDeath() {
    deaths--;
}