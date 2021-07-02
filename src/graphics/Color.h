#ifndef COLOR_H
#define COLOR_H

#include <cstdint>

typedef uint32_t Color;

namespace ColorUtils {
    Color rgba(int red, int green, int blue, int alpha = 0xFF);
    Color invert(Color c);
}

#endif