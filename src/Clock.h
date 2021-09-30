#ifndef CLOCK_H
#define CLOCK_H

#include <array>
#include <cstdint>

struct Clock final {
    typedef int64_t Nanos;

  private:
    static constexpr int BITS = 7;
    static constexpr int LENGTH = 1 << BITS;
    int index;
    Nanos last;
    Nanos sum;
    std::array<Nanos, LENGTH> time;

  public:
    Clock();
    Nanos update();
    float getUpdatesPerSecond() const;

  private:
    Nanos getNanos() const;
};

#endif