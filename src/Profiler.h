#ifndef PROFILER_H
#define PROFILER_H

#include <chrono>

#ifndef NDEBUG
namespace Profiler {
    typedef int64_t Nanos;

    void tick();
    void render();

    extern Nanos tickNanos;
    extern Nanos objectTickNanos;
    extern Nanos playerTickNanos;
    extern Nanos objectLateTickNanos;
    extern Nanos renderNanos;
    extern Nanos objectRenderNanos;
    extern Nanos objectTextRenderNanos;
    extern Nanos particleRenderNanos;

    extern Nanos postGameRenderNanos;
    extern Nanos bufferSwapNanos;
    extern Nanos eventPollNanos;

    float getFPS();
    float getTPS();

    struct Timer final {
        Nanos& n;
        Timer(Nanos& n);
        ~Timer();
    };
}
#endif

#endif