#ifndef RENDERSTATE_H
#define RENDERSTATE_H

#include "graphics/gl/Shader.h"

namespace RenderState {
    void updateViewMatrix();
    void setViewMatrix(GL::Shader& shader);
}

#endif