#include "Utils.h"

#include <cstdarg>
#include <cstdio>

#ifndef NDEBUG

void Utils::print(const char* str, ...) {
    va_list args;
    va_start(args, str);

    vprintf(str, args);

    va_end(args);
}

#else

void Utils::print(const char* str, ...) {
    (void)str;
}

#endif

void Utils::printError(const char* str, ...) {
    va_list args;
    va_start(args, str);

    vfprintf(stderr, str, args);

    va_end(args);
}
