#ifndef BUFFER_H
#define BUFFER_H

#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>

#include "NonCopyable.h"

class Buffer final : private NonCopyable {
  public:
    Buffer();
    ~Buffer();

    const void* getData() const;
    int getSize() const;
    void clear();

    template <typename T>
    Buffer& add(const T& t) {
        assert(bytes + sizeof(T) <= SIZE);
        memcpy(data + bytes, &t, sizeof(T));
        bytes += sizeof(T);
        return *this;
    }

  private:
    static constexpr int SIZE = 1024 * 1024 * 8;
    int bytes;
    char* data;
};

#endif
