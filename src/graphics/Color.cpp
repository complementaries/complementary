#include "Color.h"

Color ColorUtils::rgba(int red, int green, int blue, int alpha) {
    return (red & 0xFF) | ((green & 0xFF) << 8) | ((blue & 0xFF) << 16) | ((alpha & 0xFF) << 24);
}