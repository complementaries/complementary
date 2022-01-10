#ifndef TEXT_UTILS_H
#define TEXT_UTILS_H

#include "math/Matrix.h"
#include "math/Vector.h"

namespace TextUtils {
    void drawTimer(Vector position, long ticks);
    void drawPopupObjectSpace(Vector position, char* text, int alpha);
    void drawBestTimeObjectSpace(Vector position, long ticks, int alpha);
    void drawStartHelp(Vector position, int alpha);
}
#endif
