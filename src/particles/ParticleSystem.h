#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <vector>

#include "graphics/Color.h"
#include "math/Random.h"
#include "math/Vector.h"
#include "objects/Object.h"

enum class ParticleType {
    TRIANGLE,
    SQUARE,
    CIRCLE,
};

enum class SpawnPositionType {
    CENTER,
    BOX_EDGE,
};

struct ParticleSystemData final {
    int duration = 0;
    ParticleType type = ParticleType::SQUARE;
    int minEmissionInterval = 10;
    int maxEmissionInterval = 15;
    int minEmissionRate = 1;
    int maxEmissionRate = 1;

    Vector minStartVelocity = Vector(-0.1f, -0.1f);
    Vector maxStartVelocity = Vector(0.1f, 0.1f);
    float gravity = 0.f;
    int maxLifetime = 60;
    Color startColor = ColorUtils::rgba(255, 0, 0);
    Color endColor = ColorUtils::rgba(0, 255, 0);
    float startSize = 2;
    float endSize = 1;
    bool followPlayer = false;
    bool playOnSpawn = true;
    bool destroyOnEnd = false;
    bool enableCollision = false;
    bool clampPositionInBounds = false;
    SpawnPositionType spawnPositionType = SpawnPositionType::CENTER; // Does nothing yet
    Vector boxSize = Vector(0.5f, 0.5f);
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
    void play();
    void stop();
    void tick() override;
    void render(float lag) override;
    void renderImGui() override;
    void forceMoveParticles(const Vector& position, const Vector& size,
                            const Vector& velocity) override;

    std::shared_ptr<ObjectBase> clone() override;

    float getColliderOffset();

  private:
    std::vector<Particle> triangles;
    std::vector<Particle> squares;
    std::vector<Particle> circles;

    void renderTriangles(float lag);
    void renderSquares(float lag);
    void renderCircles(float lag);
    void tickParticles(std::vector<Particle>& particles);

    void spawnTriangle(const Vector& position, const Vector& velocity);
    void spawnSquare(const Vector& position, const Vector& velocity);
    void spawnCircle(const Vector& position, const Vector& velocity);

    bool isInBox(const Particle& particle) const;
    float getZ() const;

    int currentLifetime = 0;
    bool playing = false;
    Random random;
};

namespace ParticleRenderer {
    bool init();
    void prepare();
    void render();
}

#endif
