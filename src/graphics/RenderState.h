#ifndef RENDERSTATE_H
#define RENDERSTATE_H

#include "graphics/gl/Shader.h"
#include "math/Vector.h"

namespace RenderState {
    bool init();
    void updateViewMatrix(float lag);
    void setViewMatrix(GL::Shader& shader);
    void addShake(const Vector& v);
    void addRandomizedShake(float strength);
    void tick();

    void bindAndClearDefaultFramebuffer();

    void prepareMixer();
    void startMixing();
    void renderMixer(float lag);
}

#endif