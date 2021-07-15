#include "Buffer.h"

const void* Buffer::getData() const {
    return data.data();
}

int Buffer::getSize() const {
    return data.size();
}

void Buffer::clear() {
    data.clear();
}
