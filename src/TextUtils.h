#ifndef TEXT_UTILS_H
#define TEXT_UTILS_H

#include "math/Matrix.h"
#include "math/Vector.h"

namespace TextUtils {
    void drawTimer(Vector position, long ticks);
    void drawBestTimeObjectSpace(Vector position, long ticks, int alpha);
}
#endif
