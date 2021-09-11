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

    constexpr static Color WHITE = 0xFFFFFFFF;
    constexpr static Color LIGHT_GRAY = 0xFFC0C0C0;
    constexpr static Color GRAY = 0xFF808080;
    constexpr static Color DARK_GRAY = 0xFF404040;
    constexpr static Color BLACK = 0xFF000000;
    constexpr static Color RED = 0xFF0000FF;
    constexpr static Color PINK = 0xFFAFAFFF;
    constexpr static Color ORANGE = 0xFF00C8FF;
    constexpr static Color YELLOW = 0xFF00FFFF;
    constexpr static Color GREEN = 0xFF00FF00;
    constexpr static Color MAGENTA = 0xFFFF00FF;
    constexpr static Color CYAN = 0xFFFFFF00;
    constexpr static Color BLUE = 0xFFFF0000;
}

#endif
