#include "Color.h"

Color ColorUtils::floatRgba(float red, float green, float blue, float alpha) {
    return rgba((int)(red * 255.f), (int)(green * 255.f), (int)(blue * 255.f),
                (int)(alpha * 255.f));
}

std::tuple<int, int, int, int> ColorUtils::unpack(Color color) {
    int red = color & 0xFF;
    int green = (color >> 8) & 0xFF;
    int blue = (color >> 16) & 0xFF;
    int alpha = (color >> 24) & 0xFF;

    return {red, green, blue, alpha};
}

std::tuple<float, float, float, float> ColorUtils::unpackFloat(Color color) {
    auto [red, green, blue, alpha] = unpack(color);
    return {((float)red) / 255.f, ((float)green) / 255.f, ((float)blue) / 255.f,
            ((float)alpha) / 255.f};
}

Color ColorUtils::invert(Color c) {
    return 0xFFFFFFFFu - (c & 0xFFFFFF);
}

Color ColorUtils::mix(Color a, Color b, float factor) {
    float iFactor = 1.0f - factor;
    return rgba((a & 0xFF) * iFactor + (b & 0xFF) * factor,
                ((a >> 8) & 0xFF) * iFactor + ((b >> 8) & 0xFF) * factor,
                ((a >> 16) & 0xFF) * iFactor + ((b >> 16) & 0xFF) * factor,
                ((a >> 24) & 0xFF) * iFactor + ((b >> 24) & 0xFF) * factor);
}

Color ColorUtils::setAlpha(Color c, int alpha) {
    return (c & 0xFFFFFF) | ((alpha & 0xFF) << 24);
}
