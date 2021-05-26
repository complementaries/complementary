#!/bin/bash

SRC_FILES=$(find ./src -type f \( -iname "*.h" -o -iname "*.cpp" \))
SHADER_FILES=$(find ./assets/shaders -type f \( -iname "*.vs" -o -iname "*.fs" -o -iname "*.gs" \))
ALL_FILES="$SRC_FILES $SHADER_FILES"
clang-format -i $ALL_FILES -style=file $@
