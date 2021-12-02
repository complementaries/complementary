#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <array>
#include <vector>

#include "graphics/Color.h"
#include "math/Random.h"
#include "math/Vector.h"
#include "objects/Object.h"

enum class ParticleType {
    TRIANGLE,
    SQUARE,
    DIAMOND,
};

enum class SpawnPositionType { CENTER, BOX_EDGE, BOX, WIND, BOX_EDGE_SPIKY };

enum class Layer { BEHIND_TILEMAP, OVER_TILEMAP };

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
    float attractSpeed = 0.f;
    Layer layer = Layer::BEHIND_TILEMAP;
    bool invertColor = false;
    int boxLifetimeLoss = 0;
    Vector clampBoxSize = Vector(0, 0);
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
    void clear();
    void lateTick() override;
    void render(float lag) override;
#ifndef NDEBUG
    void renderImGui() override;
#endif
    void forceMoveParticles(const Vector& position, const Vector& size,
                            const Vector& velocity) override;
    bool allowSaving() const override;

    std::shared_ptr<ObjectBase> clone() override;

    float getColliderOffset();

    bool isPlaying() const;
    void setSpikes(const std::array<bool, 4>& spikes);

  private:
    std::vector<Particle> triangles;
    std::vector<Particle> squares;
    std::vector<Particle> diamonds;

    void renderTriangles(float lag);
    void renderSquares(float lag);
    void renderDiamonds(float lag);
    void tickParticles(std::vector<Particle>& particles);

    void spawnTriangle(const Vector& position, const Vector& velocity);
    void spawnSquare(const Vector& position, const Vector& velocity);
    void spawnDiamond(const Vector& position, const Vector& velocity);

    bool isInBox(const Particle& particle) const;
    float getZ() const;

    int currentLifetime = 0;
    bool playing = false;
    Random random;
    std::array<bool, 4> spiky;
};

namespace ParticleRenderer {
    bool init();
    void prepare();
    void render();
}

#endif
