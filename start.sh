#!/bin/bash
clear
cd "$(dirname "$0")"
cd build
if ninja; then
    echo "Build succeeded."
    cd ..
    ./build/complementary
fi
