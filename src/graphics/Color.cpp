#include "Color.h"

Color ColorUtils::rgba(int red, int green, int blue, int alpha) {
    return (red & 0xFF) | ((green & 0xFF) << 8) | ((blue & 0xFF) << 16) | ((alpha & 0xFF) << 24);
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