#ifndef TEXT_UTILS_H
#define TEXT_UTILS_H

#include "math/Matrix.h"
#include "math/Vector.h"
#include <cstdint>

namespace TextUtils {
    void drawTimer(Vector position, int64_t ticks);
    void drawPopupObjectSpace(Vector position, char* text, int alpha);
    void drawBestTimeObjectSpace(Vector position, int64_t ticks, int alpha);
    void drawStartHelp(Vector position, int alpha);
}
#endif
