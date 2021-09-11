#include "RenderState.h"

#include "graphics/Window.h"
#include "graphics/gl/Shader.h"
#include "graphics/gl/VertexBuffer.h"
#include "math/Matrix.h"
#include "math/Random.h"
#include "player/Player.h"
#include "tilemap/Tilemap.h"

#include <cmath>

static Matrix viewMatrix;
static Vector shake;
static int shakeTicks = 0;
static Random rng;

static GLuint texture = 0;
static GLuint textureDepth = 0;
static GLuint framebuffer = 0;

static GL::Shader mixer;
static GL::VertexBuffer rectangle;
static Vector mixCenter;
static float lastMixRadius = 0.0f;
static float mixRadius = 1000.0f;

// glDeleteFramebuffers(1, &fb);

bool RenderState::init() {
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, Window::getWidth(), Window::getHeight(), 0, GL_RGBA,
                 GL_FLOAT, nullptr);

    glGenTextures(1, &textureDepth);
    glBindTexture(GL_TEXTURE_2D, textureDepth);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, Window::getWidth(), Window::getHeight(), 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureDepth, 0);
    GLuint drawBuffer = GL_COLOR_ATTACHMENT0;
    glFramebufferTexture2D(GL_FRAMEBUFFER, drawBuffer, GL_TEXTURE_2D, texture, 0);
    glDrawBuffers(1, &drawBuffer);

    GLenum error = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (error != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "framebuffer error: %u\n", error);
        return true;
    }

    rectangle.init(GL::VertexBuffer::Attributes().addVector2());
    float data[] = {-1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f};
    rectangle.setData(data, sizeof(data));
    return mixer.compile({"assets/shaders/mixer.vs", "assets/shaders/mixer.fs"});
}

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
    lastMixRadius = mixRadius;
    mixRadius += 1.0f;
    if (mixRadius > 1000.0f) {
        mixRadius = 1000.0f;
    }
}

static void clear() {
    if (Player::invertColors()) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    } else {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderState::prepareMixer() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    clear();
}

void RenderState::bindAndClearDefaultFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    clear();
}

static void bindTextureTo(int textureUnit) {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, texture);
}

void RenderState::startMixing() {
    mixCenter = Player::getPosition();
    mixRadius = 0.0f;
    lastMixRadius = 0.0f;
}

void RenderState::renderMixer(float lag) {
    bindAndClearDefaultFramebuffer();
    mixer.use();
    setViewMatrix(mixer);
    Matrix texToPos;
    texToPos.transform(Vector(0.0f, Tilemap::getHeight()));
    texToPos.scale(Vector(Tilemap::getWidth(), -Tilemap::getHeight()));
    mixer.setMatrix("texToPos", texToPos);
    bindTextureTo(0);
    mixer.setVector("center", mixCenter);
    mixer.setFloat("radius", lastMixRadius + (mixRadius - lastMixRadius) * lag);
    mixer.setInt("samp", 0);
    rectangle.drawTriangles(6);
}