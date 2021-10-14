#include "Buffer.h"

char Buffer::data[Buffer::SIZE];

Buffer::Buffer() : bytes(0) /*, data(new char[SIZE])*/ {
}

Buffer::~Buffer() {
    /*delete[] data;*/
}

const void* Buffer::getData() const {
    return data;
}

int Buffer::getSize() const {
    return bytes;
}

void Buffer::clear() {
    bytes = 0;
}
