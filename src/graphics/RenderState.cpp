#include "RenderState.h"

#include "Arguments.h"
#include "Utils.h"
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

struct Framebuffer {
    GLuint id = 0;

    ~Framebuffer() {
        if (id != 0) {
            glDeleteFramebuffers(1, &id);
        }
    }
};
static Framebuffer framebuffer;

static GL::Shader mixer;
static GL::Shader lineMixer;
static GL::Shader glow;
static GL::VertexBuffer rectangle;
static Vector mixCenter;
static float lastMixRadius = 0.0f;
static float mixRadius = 1000.0f;

static float lastGlowAlpha = 0.0f;
static float glowAlpha = 0.0f;
static float lastGlowScale = 1.0f;
static float glowScale = 1.0f;

static float zoom = 1.0f;
static Vector zoomOffset = Vector();

bool RenderState::init() {
    glGenFramebuffers(1, &framebuffer.id);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Arguments::samples, GL_RGBA32F,
                            Window::getWidth(), Window::getHeight(), false);

    glGenTextures(1, &textureDepth);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureDepth);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Arguments::samples, GL_DEPTH_COMPONENT32,
                            Window::getWidth(), Window::getHeight(), false);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE,
                           textureDepth, 0);
    GLuint drawBuffer = GL_COLOR_ATTACHMENT0;
    glFramebufferTexture2D(GL_FRAMEBUFFER, drawBuffer, GL_TEXTURE_2D_MULTISAMPLE, texture, 0);
    glDrawBuffers(1, &drawBuffer);

    GLenum error = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (error != GL_FRAMEBUFFER_COMPLETE) {
        Utils::printError("framebuffer error: %u\n", error);
        return true;
    }

    rectangle.init(GL::VertexBuffer::Attributes().addVector2());
    float data[] = {-1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f};
    rectangle.setData(data, sizeof(data));
    return mixer.compile({"assets/shaders/mixer.vs", "assets/shaders/mixer.fs"}) ||
           lineMixer.compile({"assets/shaders/lineMixer.vs", "assets/shaders/lineMixer.fs"}) ||
           glow.compile({"assets/shaders/glow.vs", "assets/shaders/glow.fs"});
}

static Vector getShake(float ticks) {
    float falloff = expf(-ticks * 0.09f);
    if (falloff < 0.05f) {
        // Don't keep on wobbling for an extended time
        return Vector(0.f, 0.f);
    }
    float factor = (sinf(ticks * 0.3f) * falloff + 1.0f) * 0.5f;
    return shake * (2.0f * factor - 1.0f);
}

void RenderState::updateViewMatrix(float lag) {
    int x = Window::getWidth() / Tilemap::getWidth();
    int y = Window::getHeight() / Tilemap::getHeight();
    float factor = std::min(x, y);
    Vector realSize(Tilemap::getWidth() * factor, Tilemap::getHeight() * factor);
    viewMatrix.unit()
        .transform(realSize / Vector(-Window::getWidth(), Window::getHeight()))
        .scale(Vector(factor / Window::getWidth(), -factor / Window::getHeight()) * 2.0f);
}

void RenderState::updatePlayerViewMatrix(float lag) {
    int x = Window::getWidth() / Tilemap::getWidth();
    int y = Window::getHeight() / Tilemap::getHeight();
    float factor = std::min(x, y);
    Vector realSize(Tilemap::getWidth() * factor, Tilemap::getHeight() * factor);
    viewMatrix.unit()
        .transform(realSize / Vector(-Window::getWidth(), Window::getHeight()))
        .scale(Vector(factor / Window::getWidth(), -factor / Window::getHeight()) * 2.0f);
    viewMatrix.transform(getShake(shakeTicks + lag));

    Vector c = Player::getCenter(lag) + zoomOffset;
    viewMatrix.transform(c).scale(Vector(zoom, zoom)).transform(-c);
    float ft = std::min(zoom - 1.0f, 1.0f);
    Vector diff = (realSize / factor * 0.5f - c) / zoom * ft;
    viewMatrix.transform(diff);
}

void RenderState::updateEditorViewMatrix(float lag) {
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
    mixRadius += 1.5f + mixRadius * 0.02f;
    if (mixRadius > 1000.0f) {
        mixRadius = 1000.0f;
    }
    lastGlowAlpha = glowAlpha;
    lastGlowScale = glowScale;
    glowScale *= 1.01f;
    glowAlpha *= 0.90f;
}

void RenderState::resize(int width, int height) {
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Arguments::samples, GL_RGBA32F, width,
                            height, false);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureDepth);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Arguments::samples, GL_DEPTH_COMPONENT32,
                            width, height, false);
}

static void clear() {
    if (!Player::invertColors()) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    } else {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderState::prepareEffectFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);
    clear();
}

void RenderState::bindAndClearDefaultFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    clear();
}

static void bindTextureTo(int textureUnit) {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
}

void RenderState::startMixing() {
    mixCenter = Player::getPosition();
    mixRadius = 0.0f;
    lastMixRadius = 0.0f;
}

void RenderState::startGlowing() {
    lastGlowAlpha = 1.0f;
    glowAlpha = 1.0f;
    lastGlowScale = 1.0f;
    glowScale = 1.0f;
}

void RenderState::renderEffects(float lag) {
    bindAndClearDefaultFramebuffer();
    mixer.use();
    setViewMatrix(mixer);
    Matrix texToPos;
    texToPos.transform(Vector(0.0f, Tilemap::getHeight()));
    texToPos.scale(Vector(Tilemap::getWidth(), -Tilemap::getHeight()));
    mixer.setMatrix("texToPos", texToPos);
    mixer.setVector("center", mixCenter);
    mixer.setFloat("radius", lastMixRadius + (mixRadius - lastMixRadius) * lag);
    mixer.setInt("samp", 0);
    mixer.setInt("texels", Arguments::samples);
    bindTextureTo(0);
    rectangle.drawTriangles(6);

    enableBlending();
    glow.use();
    Matrix modifier;
    Vector modifierCenter = viewMatrix * Player::getCenter();
    modifier.transform(modifierCenter);
    float scale = lastGlowScale + (glowScale - lastGlowScale) * lag;
    modifier.scale(Vector(scale, scale));
    modifier.transform(-modifierCenter);

    glow.setMatrix("modifier", modifier);
    glow.setFloat("alpha", lastGlowAlpha + (glowAlpha - lastGlowAlpha) * lag);
    glow.setInt("samp", 0);
    glow.setInt("texels", Arguments::samples);
    bindTextureTo(0);
    rectangle.drawTriangles(6);
    disableBlending();
}

void RenderState::renderTitleScreenEffects(float lag) {
    bindAndClearDefaultFramebuffer();
    lineMixer.use();
    setViewMatrix(mixer);
    lineMixer.setInt("samp", 0);
    lineMixer.setInt("texels", Arguments::samples);
    bindTextureTo(0);
    rectangle.drawTriangles(6);
}

void RenderState::enableBlending() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
}

void RenderState::disableBlending() {
    glDisable(GL_BLEND);
}

void RenderState::setZoom(float z, Vector offset) {
    zoom = z;
    zoomOffset = offset;
}
