#include <chrono>
#include <thread>

#include "Clock.h"

Clock::Clock() : index(0), last(getNanos()), sum(0) {
    time.fill(0);
}

Clock::Nanos Clock::update() {
    index = (index + 1) & (LENGTH - 1);
    Nanos current = getNanos();
    sum -= time[index];
    time[index] = current - last;
    sum += time[index];
    last = current;
    return time[index];
}

float Clock::getUpdatesPerSecond() const {
    return (LENGTH * 1000000000.0f) / sum;
}

Clock::Nanos Clock::getNanos() const {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               std::chrono::high_resolution_clock::now().time_since_epoch())
        .count();
}