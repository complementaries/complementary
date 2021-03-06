#include "VertexBuffer.h"
#include <iostream>

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

GL::VertexBuffer::VertexBuffer()
    : vertexArray(0), vertexBuffer(0)
#ifndef NDEBUG
      ,
      vertexSize(0), dataSize(0)
#endif
{
}

GL::VertexBuffer::~VertexBuffer() {
    if (vertexBuffer != 0) {
        glDeleteBuffers(1, &vertexBuffer);
    }
    if (vertexArray != 0) {
        glDeleteVertexArrays(1, &vertexArray);
    }
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
#ifndef NDEBUG
    vertexSize = offset;
#endif
}

void GL::VertexBuffer::bindBuffer() const {
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
}

void GL::VertexBuffer::bindArray() const {
    glBindVertexArray(vertexArray);
}

void GL::VertexBuffer::setData(const void* data, int length, int dataType) {
    bindBuffer();
    glBufferData(GL_ARRAY_BUFFER, length, data, dataType);
#ifndef NDEBUG
    if (vertexSize == 0) {
        fprintf(stderr, "GL::VertexBuffer::setData before GL::VertexBuffer::init\n");
        return;
    }
    dataSize = length;
    if (dataSize % vertexSize != 0) {
        fprintf(stderr, "data length is not a multiple of vertex size: %d %d\n", dataSize,
                vertexSize);
    }
#endif
}

void GL::VertexBuffer::setStaticData(const void* data, int length) {
    setData(data, length, GL_STATIC_DRAW);
}

void GL::VertexBuffer::setStreamData(const void* data, int length) {
    setData(data, length, GL_STREAM_DRAW);
}

void setData(const void* data, int length, int dataType);

void GL::VertexBuffer::drawTriangles(int vertices, int offset) const {
    bindArray();
    glDrawArrays(GL_TRIANGLES, offset, vertices);
#ifndef NDEBUG
    if (vertexSize * vertices > dataSize) {
        fprintf(stderr, "invalid vertices on drawTriangles: %d %d %d\n", vertexSize, vertices,
                dataSize);
    }
#endif
}
