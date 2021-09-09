#ifndef COLOR_H
#define COLOR_H

#include <cstdint>
#include <tuple>

typedef uint32_t Color;

namespace ColorUtils {
    Color rgba(int red, int green, int blue, int alpha = 0xFF);
    Color rgba(float red, float green, float blue, float alpha = 1.f);
    std::tuple<int, int, int, int> unpack(Color color);
    std::tuple<float, float, float, float> unpackFloat(Color color);
    Color invert(Color c);
    Color mix(Color a, Color b, float factor);

    constexpr static Color GRAY = 0xFF7F7F7F;
}

#endif
