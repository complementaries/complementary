#ifndef RENDERSTATE_H
#define RENDERSTATE_H

#include "graphics/gl/Shader.h"
#include "math/Vector.h"

namespace RenderState {
    bool init();
    void updateViewMatrix(float lag);
    void updatePlayerViewMatrix(float lag);
    void updateEditorViewMatrix(float lag);
    void setViewMatrix(GL::Shader& shader);
    void addShake(const Vector& v);
    void addRandomizedShake(float strength);
    void tick();
    void resize(int width, int height);

    void bindAndClearDefaultFramebuffer();

    void prepareEffectFramebuffer();
    void startMixing();
    void startGlowing();
    void renderEffects(float lag);

    void enableBlending();
    void disableBlending();

    void setZoom(float zoom, Vector zoomOffset = Vector());
}

#endif
