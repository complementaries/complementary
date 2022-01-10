#include "Profiler.h"

#include <iostream>

#include "Clock.h"

#ifndef NDEBUG
static Clock tps;
static Clock fps;

static int timerTicks = 0;

static Profiler::Nanos getNanos() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               std::chrono::high_resolution_clock::now().time_since_epoch())
        .count();
}

static void tickTimer() {
    timerTicks++;
}

Profiler::Timer::Timer(Nanos& n) : n(n) {
    n = -getNanos();
}

Profiler::Timer::~Timer() {
    n += getNanos();
}

Profiler::Nanos Profiler::tickNanos = 0;
Profiler::Nanos Profiler::objectTickNanos = 0;
Profiler::Nanos Profiler::playerTickNanos = 0;
Profiler::Nanos Profiler::objectLateTickNanos = 0;
Profiler::Nanos Profiler::renderNanos = 0;
Profiler::Nanos Profiler::objectRenderNanos = 0;
Profiler::Nanos Profiler::objectTextRenderNanos = 0;
Profiler::Nanos Profiler::particleRenderNanos = 0;
Profiler::Nanos Profiler::postGameRenderNanos = 0;
Profiler::Nanos Profiler::bufferSwapNanos = 0;
Profiler::Nanos Profiler::eventPollNanos = 0;

static void logNanos(const char* msg, Profiler::Nanos n) {
    std::cout << msg << ": " << n / 1'000'000.0f << "ms\n";
}

static void logTickNanos() {
    if (tps.getUpdatesPerSecond() > 98.0f || timerTicks < 500) {
        return;
    }
    std::cout << "-------------------------------\n";
    logNanos("Tick", Profiler::tickNanos);
    logNanos("> Object Tick", Profiler::objectTickNanos);
    logNanos("> Player Tick", Profiler::playerTickNanos);
    logNanos("> Object Late Tick", Profiler::objectLateTickNanos);
}

static void logRenderNanos() {
    if (fps.getUpdatesPerSecond() > 59.0f || timerTicks < 500) {
        return;
    }
    std::cout << "-------------------------------\n";
    logNanos("Render", Profiler::renderNanos);
    logNanos("> Object Render", Profiler::objectRenderNanos);
    logNanos("> Object Text Render", Profiler::objectTextRenderNanos);
    logNanos("> Particle Render", Profiler::particleRenderNanos);
    std::cout << "-------------------------------\n";
    logNanos("Post Game Render", Profiler::postGameRenderNanos);
    logNanos("Buffer Swap", Profiler::bufferSwapNanos);
    logNanos("Event Poll", Profiler::eventPollNanos);
}

void Profiler::tick() {
    tickTimer();
    logTickNanos();
    tps.update();
}

void Profiler::render() {
    logRenderNanos();
    fps.update();
}

float Profiler::getFPS() {
    return fps.getUpdatesPerSecond();
}

float Profiler::getTPS() {
    return tps.getUpdatesPerSecond();
}

#endif
