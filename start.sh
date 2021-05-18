#!/bin/bash
clear
cd "$(dirname "$0")"
if ninja -C build; then
    echo "Build succeeded."
    ./build/complementary
fi
