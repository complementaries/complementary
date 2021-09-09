#include "TileEditorProp.h"
#ifndef NDEBUG

TileEditorProp::TileEditorProp(const char* name, TileEditorPropType type, float value, float min,
                               float max, float scale, bool disabled)
    : name(name), type(type), value(value), min(min), max(max), scale(scale), disabled(disabled) {
}

TileEditorProp TileEditorProp::Float(const char* name, float value, float min, float max,
                                     float scale, bool disabled) {
    return TileEditorProp(name, TileEditorPropType::FLOAT, value, min, max, scale, disabled);
}

TileEditorProp TileEditorProp::Int(const char* name, int value, int min, int max, bool disabled) {
    return TileEditorProp(name, TileEditorPropType::INT, value, min, max, 1.f, disabled);
}

TileEditorProp TileEditorProp::Bool(const char* name, bool value, bool disabled) {
    return TileEditorProp(name, TileEditorPropType::BOOL, value, 0, 1, 1.f, disabled);
}

#endif
