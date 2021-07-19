#include "Particles.h"

#include <cmath>
#include <vector>

#include "Game.h"
#include "graphics/Buffer.h"
#include "graphics/gl/Shader.h"
#include "graphics/gl/VertexBuffer.h"

struct Particle final {
    Vector lastPosition;
    Vector position;
    Vector velocity;
    float gravity;
    int lifetime;
    int maxLifetime;
    Color startColor;
    Color endColor;
    float startSize;
    float endSize;
};

static GL::Shader shader;
static GL::VertexBuffer buffer;
static std::vector<Particle> triangles;
static std::vector<Particle> squares;
static std::vector<Particle> circles;

bool Particles::init() {
    if (shader.compile({"assets/shaders/particle.vs", "assets/shaders/particle.fs"})) {
        return true;
    }
    buffer.init(GL::VertexBuffer::Attributes().addVector2().addRGBA());
    return false;
}

static void tick(std::vector<Particle>& particles) {
    for (unsigned int i = 0; i < particles.size(); i++) {
        Particle& p = particles[i];
        p.lastPosition = p.position;
        p.velocity[1] += p.gravity;
        p.position += p.velocity;
        p.lifetime++;
        if (p.lifetime >= p.maxLifetime) {
            particles[i] = particles.back();
            particles.erase(particles.begin() + (particles.size() - 1));
        }
    }
}

void Particles::tick() {
    if (triangles.size() < 1) {
        spawnTriangle(Vector(5.0f, 5.0f), Vector(0.15f, 0.15f), 0.0f, 200,
                      ColorUtils::rgba(0, 0, 255), ColorUtils::rgba(255, 0, 0), 5.0f, 0.0f);
    }
    if (squares.size() < 1) {
        spawnSquare(Vector(15.0f, 5.0f), Vector(0.15f, 0.15f), 0.0f, 200,
                    ColorUtils::rgba(0, 0, 255), ColorUtils::rgba(255, 0, 0), 5.0f, 0.0f);
    }
    if (circles.size() < 1) {
        spawnCircle(Vector(25.0f, 5.0f), Vector(0.15f, 0.15f), 0.0f, 200,
                    ColorUtils::rgba(0, 0, 255), ColorUtils::rgba(255, 0, 0), 5.0f, 0.0f);
    }
    tick(triangles);
    tick(squares);
    tick(circles);
}

template <typename T>
T interpolate(const T& from, const T& to, float factor) {
    return from * (1.0f - factor) + to * factor;
}

static int renderTriangles(Buffer& data, float lag) {
    int vertices = 0;
    for (Particle& p : triangles) {
        Vector position = interpolate(p.lastPosition, p.position, lag);
        float factor = static_cast<float>(p.lifetime) / p.maxLifetime;
        float size = interpolate(p.startSize, p.endSize, factor);
        float h = 0.866025f * size; // sqrtf(0.75)
        float a = 0.333333f * h;    // sin(30°) / (1 + sin(30°))
        Color c = ColorUtils::mix(p.startColor, p.endColor, factor);

        data.add(position + Vector(0.0f, a - h)).add(c);
        data.add(position + Vector(-0.5 * size, a)).add(c);
        data.add(position + Vector(0.5 * size, a)).add(c);
        vertices += 3;
    }
    return vertices;
}

static int renderSquares(Buffer& data, float lag) {
    int vertices = 0;
    for (Particle& p : squares) {
        Vector position = interpolate(p.lastPosition, p.position, lag);
        float factor = static_cast<float>(p.lifetime) / p.maxLifetime;
        float halfSize = 0.5f * interpolate(p.startSize, p.endSize, factor);
        Color c = ColorUtils::mix(p.startColor, p.endColor, factor);

        Vector leftBottom = position + Vector(-halfSize, halfSize);
        Vector rightTop = position + Vector(halfSize, -halfSize);

        data.add(position - Vector(halfSize, halfSize)).add(c);
        data.add(leftBottom).add(c);
        data.add(rightTop).add(c);
        data.add(position + Vector(halfSize, halfSize)).add(c);
        data.add(leftBottom).add(c);
        data.add(rightTop).add(c);
        vertices += 6;
    }
    return vertices;
}

static int renderCircles(Buffer& data, float lag) {
    int vertices = 0;
    for (Particle& p : circles) {
        Vector position = interpolate(p.lastPosition, p.position, lag);
        float factor = static_cast<float>(p.lifetime) / p.maxLifetime;
        float radius = 0.5f * interpolate(p.startSize, p.endSize, factor);
        Color c = ColorUtils::mix(p.startColor, p.endColor, factor);

        Vector last = position + Vector(0.0f, radius);
        constexpr float full = 6.283185307f;
        float step = full / radius * 0.05f;
        for (float f = step; f <= full + step; f += step) {
            Vector rotated = position + Vector(sinf(f), cosf(f)) * radius;
            data.add(position).add(c);
            data.add(rotated).add(c);
            data.add(last).add(c);
            last = rotated;
            vertices += 3;
        }
    }
    return vertices;
}

void Particles::render(float lag) {
    shader.use();
    shader.setMatrix("view", Game::viewMatrix);

    Buffer data;
    int vertices = renderTriangles(data, lag);
    vertices += renderSquares(data, lag);
    vertices += renderCircles(data, lag);
    buffer.setData(data.getData(), data.getSize());
    buffer.drawTriangles(vertices);
}

void Particles::spawnTriangle(const Vector& position, const Vector& velocity, float gravity,
                              int lifetime, Color startColor, Color endColor, float startSize,
                              float endSize) {
    triangles.push_back({position, position, velocity, gravity, 0, lifetime, startColor, endColor,
                         startSize, endSize});
}

void Particles::spawnSquare(const Vector& position, const Vector& velocity, float gravity,
                            int lifetime, Color startColor, Color endColor, float startSize,
                            float endSize) {
    squares.push_back({position, position, velocity, gravity, 0, lifetime, startColor, endColor,
                       startSize, endSize});
}

void Particles::spawnCircle(const Vector& position, const Vector& velocity, float gravity,
                            int lifetime, Color startColor, Color endColor, float startSize,
                            float endSize) {
    circles.push_back({position, position, velocity, gravity, 0, lifetime, startColor, endColor,
                       startSize, endSize});
}