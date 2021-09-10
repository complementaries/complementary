#include "ParticleSystem.h"

#include <cassert>
#include <cmath>
#include <imgui.h>
#include <vector>

#include "Game.h"
#include "imgui/ImGuiUtils.h"
#include "player/Player.h"

ParticleSystem::ParticleSystem() {
    assert(!shader.compile({"assets/shaders/particle.vs", "assets/shaders/particle.fs"}));
    buffer.init(GL::VertexBuffer::Attributes().addVector2().addRGBA());
}

ParticleSystem::ParticleSystem(Vector position) : ParticleSystem() {
    this->position = position;
}

ParticleSystem::ParticleSystem(const ParticleSystemData& data) : ParticleSystem() {
    this->data = data;
}

void ParticleSystem::tickParticles(std::vector<Particle>& particles) {
    for (unsigned int i = 0; i < particles.size(); i++) {
        Particle& p = particles[i];
        p.lastPosition = p.position;
        p.velocity[1] += data.gravity;
        p.position += p.velocity;
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

    if ((data.duration <= 0.f || currentLifetime < data.duration) &&
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
    if (data.duration > 0.f && currentLifetime >= data.duration + data.maxLifetime) {
        destroy();
    }
}

template <typename T>
T interpolate(const T& from, const T& to, float factor) {
    return from * (1.0f - factor) + to * factor;
}

int ParticleSystem::renderTriangles(Buffer& buffer, float lag) {
    int vertices = 0;
    for (Particle& p : triangles) {
        Vector particlePosition = interpolate(p.lastPosition, p.position, lag);
        float factor = static_cast<float>(p.lifetime) / data.maxLifetime;
        float size = interpolate(data.startSize, data.endSize, factor);
        float h = 0.866025f * size; // sqrtf(0.75)
        float a = 0.333333f * h;    // sin(30°) / (1 + sin(30°))
        Color c = ColorUtils::mix(data.startColor, data.endColor, factor);

        buffer.add(particlePosition + Vector(0.0f, a - h)).add(c);
        buffer.add(particlePosition + Vector(-0.5 * size, a)).add(c);
        buffer.add(particlePosition + Vector(0.5 * size, a)).add(c);
        vertices += 3;
    }
    return vertices;
}

int ParticleSystem::renderSquares(Buffer& buffer, float lag) {
    int vertices = 0;
    for (Particle& p : squares) {
        Vector particlePosition = interpolate(p.lastPosition, p.position, lag);
        float factor = static_cast<float>(p.lifetime) / data.maxLifetime;
        float halfSize = 0.5f * interpolate(data.startSize, data.endSize, factor);
        Color c = ColorUtils::mix(data.startColor, data.endColor, factor);
        auto [r, g, b, a] = ColorUtils::unpack(c);
        printf("factor=%f, r=%d, g=%d, b=%d, a=%d\n", factor, r, g, b, a);

        Vector leftBottom = particlePosition + Vector(-halfSize, halfSize);
        Vector rightTop = particlePosition + Vector(halfSize, -halfSize);

        buffer.add(particlePosition - Vector(halfSize, halfSize)).add(c);
        buffer.add(leftBottom).add(c);
        buffer.add(rightTop).add(c);
        buffer.add(particlePosition + Vector(halfSize, halfSize)).add(c);
        buffer.add(leftBottom).add(c);
        buffer.add(rightTop).add(c);
        vertices += 6;
    }
    return vertices;
}

int ParticleSystem::renderCircles(Buffer& buffer, float lag) {
    int vertices = 0;
    for (Particle& p : circles) {
        Vector particlePosition = interpolate(p.lastPosition, p.position, lag);
        float factor = static_cast<float>(p.lifetime) / data.maxLifetime;
        float radius = 0.5f * interpolate(data.startSize, data.endSize, factor);
        Color c = ColorUtils::mix(data.startColor, data.endColor, factor);

        Vector last = particlePosition + Vector(0.0f, radius);
        constexpr float full = 6.283185307f;
        float step = full / radius * 0.05f;
        for (float f = step; f <= full + step; f += step) {
            Vector rotated = particlePosition + Vector(sinf(f), cosf(f)) * radius;
            buffer.add(particlePosition).add(c);
            buffer.add(rotated).add(c);
            buffer.add(last).add(c);
            last = rotated;
            vertices += 3;
        }
    }
    return vertices;
}

void ParticleSystem::render(float lag) {
    shader.use();
    shader.setMatrix("view", Game::viewMatrix);

    Buffer data;
    int vertices = renderTriangles(data, lag);
    vertices += renderSquares(data, lag);
    vertices += renderCircles(data, lag);
    buffer.setData(data.getData(), data.getSize());
    buffer.drawTriangles(vertices);
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

std::shared_ptr<ObjectBase> ParticleSystem::clone() {
    return std::make_shared<ParticleSystem>(data);
}

void ParticleSystem::renderImGui() {
    ImGui::DragInt("Duration", &data.duration);
    const char* types[3];
    types[0] = "Triangle";
    types[1] = "Square";
    types[2] = "Circle";
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

    ImGui::Text("Current duration: %d, particle count: %zu", currentLifetime,
                triangles.size() + squares.size() + circles.size());
}
