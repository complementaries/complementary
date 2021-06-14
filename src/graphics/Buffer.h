#ifndef BUFFER_H
#define BUFFER_H

#include <vector>

class Buffer final {
  public:
    const void* getData() const;
    int getSize() const;

    template <typename T>
    Buffer& add(const T& t) {
        const char* raw = reinterpret_cast<const char*>(&t);
        for (unsigned int i = 0; i < sizeof(T); i++) {
            data.push_back(raw[i]);
        }
        return *this;
    }

  private:
    std::vector<char> data;
};

#endif