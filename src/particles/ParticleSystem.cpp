#include "ParticleSystem.h"

#include <cmath>
#include <imgui.h>
#include <vector>

#include "graphics/Buffer.h"
#include "graphics/RenderState.h"
#include "graphics/gl/Shader.h"
#include "graphics/gl/VertexBuffer.h"
#include "imgui/ImGuiUtils.h"
#include "player/Player.h"

static GL::Shader shader;
static GL::VertexBuffer buffer;
static Buffer rawData;
static int vertices = 0;

bool ParticleRenderer::init() {
    buffer.init(GL::VertexBuffer::Attributes().addVector2().addRGBA());
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

void ParticleSystem::tickParticles(std::vector<Particle>& particles) {
    for (unsigned int i = 0; i < particles.size(); i++) {
        Particle& p = particles[i];
        p.lastPosition = p.position;
        p.velocity[1] += data.gravity;
        auto nextPosition = p.position + p.velocity;
        if (!data.clampPositionInBounds || isInBox(p)) {
            p.position = nextPosition;
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
        position = Player::getPosition();
    }

    if (playing && (data.duration <= 0.f || currentLifetime < data.duration) &&
        (data.emissionInterval <= 0 || currentLifetime % data.emissionInterval == 0)) {
        for (int i = 0; i < data.emissionRate; i++) {
            float startVelocityX =
                random.nextFloat(data.minStartVelocity.x, data.maxStartVelocity.x);
            float startVelocityY =
                random.nextFloat(data.minStartVelocity.y, data.maxStartVelocity.y);
            Vector startVelocity(startVelocityX, startVelocityY);
            switch (data.type) {
                case ParticleType::TRIANGLE: spawnTriangle(position, startVelocity); break;
                case ParticleType::SQUARE: spawnSquare(position, startVelocity); break;
                case ParticleType::CIRCLE: spawnCircle(position, startVelocity); break;
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

template <typename T>
T interpolate(const T& from, const T& to, float factor) {
    return from * (1.0f - factor) + to * factor;
}

void ParticleSystem::renderTriangles(float lag) {
    for (Particle& p : triangles) {
        Vector particlePosition = interpolate(p.lastPosition, p.position, lag);
        float factor = (p.lifetime + lag) / data.maxLifetime;
        float size = interpolate(data.startSize, data.endSize, factor);
        float h = 0.866025f * size; // sqrtf(0.75)
        float a = 0.333333f * h;    // sin(30°) / (1 + sin(30°))
        Color c = ColorUtils::mix(data.startColor, data.endColor, factor);

        rawData.add(particlePosition + Vector(0.0f, a - h)).add(c);
        rawData.add(particlePosition + Vector(-0.5 * size, a)).add(c);
        rawData.add(particlePosition + Vector(0.5 * size, a)).add(c);
        vertices += 3;
    }
}

void ParticleSystem::renderSquares(float lag) {
    for (Particle& p : squares) {
        Vector particlePosition = interpolate(p.lastPosition, p.position, lag);
        float factor = (p.lifetime + lag) / data.maxLifetime;
        float halfSize = 0.5f * interpolate(data.startSize, data.endSize, factor);
        Color c = ColorUtils::mix(data.startColor, data.endColor, factor);

        Vector leftBottom = particlePosition + Vector(-halfSize, halfSize);
        Vector rightTop = particlePosition + Vector(halfSize, -halfSize);

        rawData.add(particlePosition - Vector(halfSize, halfSize)).add(c);
        rawData.add(leftBottom).add(c);
        rawData.add(rightTop).add(c);
        rawData.add(particlePosition + Vector(halfSize, halfSize)).add(c);
        rawData.add(leftBottom).add(c);
        rawData.add(rightTop).add(c);
        vertices += 6;
    }
}

void ParticleSystem::renderCircles(float lag) {
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
            rawData.add(particlePosition).add(c);
            rawData.add(rotated).add(c);
            rawData.add(last).add(c);
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
    return particle.position.x > position.x - data.boxSize.x &&
           particle.position.x < position.x + data.boxSize.x &&
           particle.position.y > position.y - data.boxSize.y &&
           particle.position.y < position.y + data.boxSize.y;
}

std::shared_ptr<ObjectBase> ParticleSystem::clone() {
    return std::make_shared<ParticleSystem>(data);
}

void ParticleSystem::renderImGui() {
    ImGui::DragInt("Duration", &data.duration);
    const char* types[] = {"Triangle", "Square", "Circle"};
    ImGui::ListBox("Particle type", (int*)&data.type, types, 3);
    ImGui::DragInt("Emission Interval", &data.emissionInterval);
    ImGui::DragInt("Emission Rate", &data.emissionRate);
    ImGui::Spacing();

    ImGui::DragFloat2("Min start velocity", data.minStartVelocity.data());
    ImGui::DragFloat2("Max start velocity", data.maxStartVelocity.data());
    ImGui::DragFloat("Gravity", &data.gravity);
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
