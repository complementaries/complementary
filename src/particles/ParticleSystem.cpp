#include "ParticleSystem.h"

#include <cmath>
#include <imgui.h>
#include <vector>

#include "graphics/Buffer.h"
#include "graphics/RenderState.h"
#include "graphics/gl/Shader.h"
#include "graphics/gl/VertexBuffer.h"
#include "imgui/ImGuiUtils.h"
#include "objects/Objects.h"
#include "player/Player.h"
#include "tilemap/Tilemap.h"
#include "tilemap/Tiles.h"

static GL::Shader shader;
static GL::VertexBuffer buffer;
static Buffer rawData;
static int vertices = 0;
static float collisionFactor = 0.9f;
constexpr float step = 0.01f;

bool ParticleRenderer::init() {
    buffer.init(GL::VertexBuffer::Attributes().addVector3().addRGBA());
    return shader.compile({"assets/shaders/particle.vs", "assets/shaders/particle.fs"});
}

void ParticleRenderer::prepare() {
    rawData.clear();
    vertices = 0;
}

void ParticleRenderer::render() {
    shader.use();
    RenderState::setViewMatrix(shader);
    buffer.setData(rawData.getData(), rawData.getSize());
    buffer.drawTriangles(vertices);
}

ParticleSystem::ParticleSystem() {
}

ParticleSystem::ParticleSystem(Vector position) : ParticleSystem() {
    this->position = position;
    if (data.playOnSpawn) {
        play();
    }
}

ParticleSystem::ParticleSystem(const ParticleSystemData& data) : ParticleSystem() {
    this->data = data;
}

void ParticleSystem::play() {
    this->playing = true;
    currentLifetime = 0.f;
}

void ParticleSystem::stop() {
    this->playing = false;
}

float ParticleSystem::getColliderOffset() {
    return this->data.startSize / 2.0f * collisionFactor;
}

template <typename T>
T interpolate(const T& from, const T& to, float factor) {
    return from * (1.0f - factor) + to * factor;
}

static bool isColliding(const ParticleSystemData& s, Particle& p, const Vector& position,
                        const Vector& size) {
    float factor = static_cast<float>(p.lifetime) / s.maxLifetime;
    float halfSize = 0.5f * interpolate(s.startSize, s.endSize, factor) * collisionFactor;
    float minX = p.position.x - halfSize;
    float minY = p.position.y - halfSize;
    float maxX = p.position.x + halfSize;
    float maxY = p.position.y + halfSize;
    return minX < position[0] + size[0] && maxX > position[0] && minY < position[1] + size[1] &&
           maxY > position[1];
}

static bool isColliding(const ParticleSystemData& s, Particle& p) {
    float factor = static_cast<float>(p.lifetime) / s.maxLifetime;
    float halfSize = 0.5f * interpolate(s.startSize, s.endSize, factor);
    int minX = floorf(p.position.x - halfSize);
    int minY = floorf(p.position.y - halfSize);
    int maxX = floorf(p.position.x + halfSize);
    int maxY = floorf(p.position.y + halfSize);
    if (minX < 0 || minY < 0 || maxX >= Tilemap::getWidth() || maxY >= Tilemap::getHeight()) {
        return true;
    }
    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            const Tile& tile = Tilemap::getTile(x, y);
            if (tile == Tiles::SPIKES) {
                // Make the hitbox of spikes a bit smaller because it looks weird if
                // the particles hover above the spike triangles
                return p.position.y > y + 0.25f;
            }
            if (tile.isSolid()) {
                return true;
            }
        }
    }
    Vector half(halfSize, halfSize);
    return Objects::collidesWithAny(p.position - half, half * 2.0f);
}

static void move(const ParticleSystemData& s, Particle& p) {
    Vector energy = p.velocity;
    while (energy[0] != 0.0f || energy[1] != 0.0f) {
        for (int i = 0; i < 2; i++) {
            if (energy[i] == 0.0f) {
                continue;
            }
            float old = p.position[i];
            if (energy[i] > step) {
                p.position[i] += step;
                energy[i] -= step;
            } else if (energy[i] < -step) {
                p.position[i] -= step;
                energy[i] += step;
            } else {
                p.position[i] += energy[i];
                energy[i] = 0.0f;
            }
            if (isColliding(s, p)) {
                energy[i] = 0.0f;
                p.position[i] = old;
                p.velocity[i] = 0.0f;
            }
        }
    }
}

void ParticleSystem::tickParticles(std::vector<Particle>& particles) {
    for (unsigned int i = 0; i < particles.size(); i++) {
        Particle& p = particles[i];
        p.lastPosition = p.position;
        p.velocity[1] += data.gravity;
        p.velocity += (position - p.position) * data.attractSpeed;
        Vector nextPosition = p.position + p.velocity;
        if (!data.clampPositionInBounds || isInBox(p)) {
            if (data.enableCollision) {
                move(data, p);
            } else {
                p.position = nextPosition;
            }
        }

        p.lifetime++;
        if (p.lifetime >= data.maxLifetime) {
            particles[i] = particles.back();
            particles.erase(particles.begin() + (particles.size() - 1));
        }
    }
}

void ParticleSystem::tick() {
    if (data.followPlayer) {
        position = Player::getCenter();
    }

    Vector particlePosition = position;

    if (playing && (data.duration <= 0.f || currentLifetime < data.duration)) {
        int emissionInterval = random.next(data.minEmissionInterval, data.maxEmissionInterval + 1);
        if (emissionInterval <= 0 || currentLifetime % emissionInterval == 0) {
            int emissionRate = random.next(data.minEmissionRate, data.maxEmissionRate + 1);
            for (int i = 0; i < emissionRate; i++) {
                if (data.spawnPositionType == SpawnPositionType::BOX_EDGE) {
                    Face spawnFace = static_cast<Face>(random.next(0, static_cast<int>(Face::MAX)));
                    if (spawnFace == Face::LEFT) {
                        particlePosition.x = position.x - data.boxSize.x * 0.5f;
                        particlePosition.y = position.y + random.nextFloat(-data.boxSize.y * 0.5f,
                                                                           data.boxSize.y * 0.5f);
                    } else if (spawnFace == Face::RIGHT) {
                        particlePosition.x = position.x + data.boxSize.x * 0.5f;
                        particlePosition.y = position.y + random.nextFloat(-data.boxSize.y * 0.5f,
                                                                           data.boxSize.y * 0.5f);
                    } else if (spawnFace == Face::UP) {
                        particlePosition.x = position.x + random.nextFloat(-data.boxSize.x * 0.5f,
                                                                           data.boxSize.x * 0.5f);
                        particlePosition.y = position.y - data.boxSize.y * 0.5f;
                    } else if (spawnFace == Face::DOWN) {
                        particlePosition.x = position.x + random.nextFloat(-data.boxSize.x * 0.5f,
                                                                           data.boxSize.x * 0.5f);
                        particlePosition.y = position.y + data.boxSize.y * 0.5f;
                    }
                }

                float startVelocityX =
                    random.nextFloat(data.minStartVelocity.x, data.maxStartVelocity.x);
                float startVelocityY =
                    random.nextFloat(data.minStartVelocity.y, data.maxStartVelocity.y);
                Vector startVelocity(startVelocityX, startVelocityY);
                switch (data.type) {
                    case ParticleType::TRIANGLE:
                        spawnTriangle(particlePosition, startVelocity);
                        break;
                    case ParticleType::SQUARE: spawnSquare(particlePosition, startVelocity); break;
                    case ParticleType::CIRCLE: spawnCircle(particlePosition, startVelocity); break;
                }
            }
        }
    }

    tickParticles(triangles);
    tickParticles(squares);
    tickParticles(circles);

    currentLifetime++;
    if (data.destroyOnEnd && data.duration > 0.f &&
        currentLifetime >= data.duration + data.maxLifetime) {
        destroy();
    }
}

float ParticleSystem::getZ() const {
    return data.enableCollision ? -0.1f : -0.1f;
}

void ParticleSystem::renderTriangles(float lag) {
    float z = getZ();
    for (Particle& p : triangles) {
        Vector particlePosition = interpolate(p.lastPosition, p.position, lag);
        float factor = (p.lifetime + lag) / data.maxLifetime;
        float size = interpolate(data.startSize, data.endSize, factor);
        float h = 0.866025f * size; // sqrtf(0.75)
        float a = 0.333333f * h;    // sin(30°) / (1 + sin(30°))
        Color c = ColorUtils::mix(data.startColor, data.endColor, factor);

        rawData.add(particlePosition + Vector(0.0f, a - h)).add(z).add(c);
        rawData.add(particlePosition + Vector(-0.5 * size, a)).add(z).add(c);
        rawData.add(particlePosition + Vector(0.5 * size, a)).add(z).add(c);
        vertices += 3;
    }
}

void ParticleSystem::renderSquares(float lag) {
    float z = getZ();
    for (Particle& p : squares) {
        Vector particlePosition = interpolate(p.lastPosition, p.position, lag);
        float factor = (p.lifetime + lag) / data.maxLifetime;
        float halfSize = 0.5f * interpolate(data.startSize, data.endSize, factor);
        Color c = ColorUtils::mix(data.startColor, data.endColor, factor);

        Vector leftBottom = particlePosition + Vector(-halfSize, halfSize);
        Vector rightTop = particlePosition + Vector(halfSize, -halfSize);

        rawData.add(particlePosition - Vector(halfSize, halfSize)).add(z).add(c);
        rawData.add(leftBottom).add(z).add(c);
        rawData.add(rightTop).add(z).add(c);
        rawData.add(particlePosition + Vector(halfSize, halfSize)).add(z).add(c);
        rawData.add(leftBottom).add(z).add(c);
        rawData.add(rightTop).add(z).add(c);
        vertices += 6;
    }
}

void ParticleSystem::renderCircles(float lag) {
    float z = getZ();
    for (Particle& p : circles) {
        Vector particlePosition = interpolate(p.lastPosition, p.position, lag);
        float factor = (p.lifetime + lag) / data.maxLifetime;
        float radius = 0.5f * interpolate(data.startSize, data.endSize, factor);
        Color c = ColorUtils::mix(data.startColor, data.endColor, factor);

        Vector last = particlePosition + Vector(0.0f, radius);
        constexpr float full = 6.283185307f;
        float step = full / radius * 0.05f;
        for (float f = step; f <= full + step; f += step) {
            Vector rotated = particlePosition + Vector(sinf(f), cosf(f)) * radius;
            rawData.add(particlePosition).add(z).add(c);
            rawData.add(rotated).add(z).add(c);
            rawData.add(last).add(z).add(c);
            last = rotated;
            vertices += 3;
        }
    }
}

void ParticleSystem::render(float lag) {
    renderTriangles(lag);
    renderSquares(lag);
    renderCircles(lag);
}

void ParticleSystem::spawnTriangle(const Vector& position, const Vector& velocity) {
    triangles.push_back({position, position, velocity, 0});
}

void ParticleSystem::spawnSquare(const Vector& position, const Vector& velocity) {
    squares.push_back({position, position, velocity, 0});
}

void ParticleSystem::spawnCircle(const Vector& position, const Vector& velocity) {
    circles.push_back({position, position, velocity, 0});
}

bool ParticleSystem::isInBox(const Particle& particle) const {
    return particle.position.x > position.x - data.boxSize.x * 0.5f &&
           particle.position.x < position.x + data.boxSize.x * 0.5f &&
           particle.position.y > position.y - data.boxSize.y * 0.5f &&
           particle.position.y < position.y + data.boxSize.y * 0.5f;
}

std::shared_ptr<ObjectBase> ParticleSystem::clone() {
    return std::make_shared<ParticleSystem>(data);
}

void ParticleSystem::renderImGui() {
    ImGui::DragInt("Duration", &data.duration);
    const char* types[] = {"Triangle", "Square", "Circle"};
    ImGui::ListBox("Particle type", (int*)&data.type, types, 3);
    ImGui::DragInt2("Emission Interval", &data.minEmissionInterval);
    ImGui::DragInt2("Emission Rate", &data.minEmissionRate);
    ImGui::Spacing();

    ImGui::DragFloat2("Min start velocity", data.minStartVelocity.data(), 0.05f);
    ImGui::DragFloat2("Max start velocity", data.maxStartVelocity.data(), 0.05f);
    ImGui::DragFloat("Gravity", &data.gravity);
    ImGui::DragFloat("Attract speed", &data.attractSpeed, 0.01f);

    ImGui::DragInt("Particle lifetime", &data.maxLifetime);
    ImGuiUtils::ColorPicker("Start color", &data.startColor);
    ImGuiUtils::ColorPicker("End color", &data.endColor);
    ImGui::DragFloat2("Size over lifetime", &data.startSize);
    ImGui::Checkbox("Follow player", &data.followPlayer);
    ImGui::Checkbox("Play on spawn", &data.playOnSpawn);
    ImGui::Checkbox("Destroy on end", &data.destroyOnEnd);
    ImGui::Checkbox("Enable collision", &data.enableCollision);
    ImGui::Checkbox("Clamp pos. in bounds", &data.clampPositionInBounds);

    const char* spawnPosTypes[] = {"Center", "Box Edge"};
    ImGui::ListBox("Spawn pos type", (int*)&data.spawnPositionType, spawnPosTypes, 2);
    ImGui::DragFloat2("Box Size", data.boxSize.data());

    ImGui::Text("Current duration: %d, particle count: %zu", currentLifetime,
                triangles.size() + squares.size() + circles.size());

    if (ImGui::Button("Play")) {
        play();
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop")) {
        stop();
    }
}

void ParticleSystem::forceMoveParticles(const Vector& position, const Vector& size,
                                        const Vector& velocity) {
    for (Particle& p : squares) {
        if (isColliding(data, p, position, size)) {
            p.position += velocity;
            p.lastPosition += velocity;
        }
    }
}
