#include <chrono>

#include "Random.h"

Random::Random(Seed seed) : index(0) {
    for (int i = 0; i < N; i++) {
        data[i] = seed;
        seed = seed * 7 + 31;
    }
}

Random::Random() : Random(std::chrono::steady_clock::now().time_since_epoch().count()) {
}

void Random::update() {
    static const Seed map[2] = {0, 0x8EBFD028};
    for (int i = 0; i < N - M; i++) {
        data[i] = data[i + M] ^ (data[i] >> 1) ^ map[data[i] & 1];
    }
    for (int i = N - M; i < N; i++) {
        data[i] = data[i + (M - N)] ^ (data[i] >> 1) ^ map[data[i] & 1];
    }
    index = 0;
}

int Random::next() {
    if (index >= N) {
        update();
    }
    Seed r = data[index++];
    r ^= (r << 7) & 0x2B5B2500;
    r ^= (r << 15) & 0xDB8B0000;
    r ^= (r >> 16);
    return static_cast<int>(r >> 1);
}

int Random::next(int min, int exclusiveMax) {
    return min + next() % (exclusiveMax - min);
}

float Random::nextFloat() {
    static constexpr int bits = sizeof(int) * 6;
    static constexpr int mask = (1 << bits) - 1;
    return (next() & mask) * (1.0f / (1.0f + mask));
}

float Random::nextFloat(float min, float exclusiveMax) {
    return min + nextFloat() * (exclusiveMax - min);
}
