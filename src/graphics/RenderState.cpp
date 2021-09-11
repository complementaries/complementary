#include "RenderState.h"

#include "math/Matrix.h"
#include "math/Random.h"
#include "tilemap/Tilemap.h"

#include <cmath>

static Matrix viewMatrix;
static Vector shake;
static int shakeTicks = 0;
static Random rng;

static Vector getShake(float ticks) {
    float factor = (sinf(ticks * 0.125f) * expf(-ticks * 0.025f) + 1.0f) * 0.5f;
    return shake * (2.0f * factor - 1.0f);
}

void RenderState::updateViewMatrix(float lag) {
    viewMatrix.unit()
        .transform(Vector(-1.0f, 1.0f))
        .scale(Vector(2.0f / Tilemap::getWidth(), -2.0f / Tilemap::getHeight()));
    viewMatrix.transform(getShake(shakeTicks + lag));
}

void RenderState::setViewMatrix(GL::Shader& shader) {
    shader.setMatrix("view", viewMatrix);
}

void RenderState::addShake(const Vector& v) {
    shake = getShake(shakeTicks);
    shake += v;
    shakeTicks = 0;
}

void RenderState::addRandomizedShake(float strength) {
    float angle = rng.nextFloat(0.0f, 6.283f);
    addShake(Vector(sinf(angle), cos(angle)) * strength);
}

void RenderState::tick() {
    shakeTicks++;
}