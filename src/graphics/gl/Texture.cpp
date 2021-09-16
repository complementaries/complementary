#include "Texture.h"

GL::Texture::Texture() : texture(0) {
}

GL::Texture::~Texture() {
    glDeleteTextures(1, &texture);
}

void GL::Texture::init() {
    glGenTextures(1, &texture);
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void GL::Texture::setData(int width, int height, const void* data) {
    bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

void GL::Texture::bind() const {
    glBindTexture(GL_TEXTURE_2D, texture);
}

void GL::Texture::bindTo(int index) const {
    glActiveTexture(GL_TEXTURE0 + index);
    bind();
}