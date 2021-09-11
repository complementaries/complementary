#include "RenderState.h"

#include "math/Matrix.h"
#include "tilemap/Tilemap.h"

static Matrix viewMatrix;

void RenderState::updateViewMatrix() {
    viewMatrix.unit()
        .transform(Vector(-1.0f, 1.0f))
        .scale(Vector(2.0f / Tilemap::getWidth(), -2.0f / Tilemap::getHeight()));
}

void RenderState::setViewMatrix(GL::Shader& shader) {
    shader.setMatrix("view", viewMatrix);
}