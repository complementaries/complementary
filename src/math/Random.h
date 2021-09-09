#ifndef RANDOM_H
#define RANDOM_H

#include <cstdint>

struct Random final {
    typedef uint32_t Seed;

  private:
    constexpr static int N = 25;
    constexpr static int M = 7;

    Seed data[N];
    int index;

    void update();

  public:
    Random(Seed seed);
    Random();

    int next();
    int next(int min, int exclusiveMax);
    float nextFloat();
    float nextFloat(float min, float exclusiveMax);
};

#endif
