#include "VertexBuffer.h"

GL::VertexBuffer::Attributes& GL::VertexBuffer::Attributes::addFloat(int count) {
    data.push_back({GL_FLOAT, false, count, static_cast<int>(sizeof(float) * count)});
    return *this;
}

GL::VertexBuffer::Attributes& GL::VertexBuffer::Attributes::addFloat() {
    return addFloat(1);
}

GL::VertexBuffer::Attributes& GL::VertexBuffer::Attributes::addVector2() {
    return addFloat(2);
}

GL::VertexBuffer::Attributes& GL::VertexBuffer::Attributes::addVector3() {
    return addFloat(3);
}

GL::VertexBuffer::Attributes& GL::VertexBuffer::Attributes::addVector4() {
    return addFloat(4);
}

GL::VertexBuffer::Attributes& GL::VertexBuffer::Attributes::addRGBA() {
    data.push_back({GL_UNSIGNED_BYTE, true, 4, 4});
    return *this;
}

GL::VertexBuffer::VertexBuffer() : vertexArray(0), vertexBuffer(0) {
}

GL::VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vertexArray);
}

void GL::VertexBuffer::init(const Attributes& a) {
    glGenVertexArrays(1, &vertexArray);
    bindArray();

    glGenBuffers(1, &vertexBuffer);
    bindBuffer();

    int size = 0;
    for (const Attributes::Data& data : a.data) {
        size += data.size;
    }
    int offset = 0;
    for (unsigned int i = 0; i < a.data.size(); i++) {
        const Attributes::Data& d = a.data[i];
        constexpr char* o = nullptr;
        glVertexAttribPointer(i, d.count, d.type, d.normalized, size, o + offset);
        glEnableVertexAttribArray(i);
        offset += d.size;
    }
}

void GL::VertexBuffer::bindBuffer() const {
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
}

void GL::VertexBuffer::bindArray() const {
    glBindVertexArray(vertexArray);
}

void GL::VertexBuffer::setData(const void* data, int length) {
    bindBuffer();
    glBufferData(GL_ARRAY_BUFFER, length, data, GL_STATIC_DRAW);
}

void GL::VertexBuffer::drawTriangles(int vertices) const {
    bindArray();
    glDrawArrays(GL_TRIANGLES, 0, vertices);
}
