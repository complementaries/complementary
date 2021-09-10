#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <vector>

#include "graphics/Buffer.h"
#include "graphics/Color.h"
#include "graphics/gl/Shader.h"
#include "graphics/gl/VertexBuffer.h"
#include "math/Random.h"
#include "math/Vector.h"
#include "objects/Object.h"

enum class ParticleType {
    TRIANGLE,
    SQUARE,
    CIRCLE,
};

struct ParticleSystemData final {
    int duration = 0;
    ParticleType type = ParticleType::SQUARE;
    int emissionInterval = 15;
    int emissionRate = 1;

    Vector minStartVelocity = Vector(-0.1f, -0.1f);
    Vector maxStartVelocity = Vector(0.1f, 0.1f);
    float gravity = 0.f;
    int maxLifetime = 60;
    Color startColor = ColorUtils::rgba(255, 0, 0);
    Color endColor = ColorUtils::rgba(0, 255, 0);
    float startSize = 2;
    float endSize = 1;
    bool followPlayer = false;
};

struct Particle final {
    Vector lastPosition;
    Vector position;
    Vector velocity;
    int lifetime;
};

class ParticleSystem : public Object<ParticleSystemData> {
  public:
    ParticleSystem();
    ParticleSystem(Vector position);
    ParticleSystem(const ParticleSystemData& data);
    void tick() override;
    void render(float lag) override;
    void renderImGui() override;

    void spawnTriangle(const Vector& position, const Vector& velocity);
    void spawnSquare(const Vector& position, const Vector& velocity);
    void spawnCircle(const Vector& position, const Vector& velocity);
    std::shared_ptr<ObjectBase> clone() override;

  private:
    GL::Shader shader;
    GL::VertexBuffer buffer;
    std::vector<Particle> triangles;
    std::vector<Particle> squares;
    std::vector<Particle> circles;

    int renderTriangles(Buffer& data, float lag);
    int renderSquares(Buffer& data, float lag);
    int renderCircles(Buffer& data, float lag);
    void tickParticles(std::vector<Particle>& particles);

    int currentLifetime = 0;
    Random random;
};

#endif
