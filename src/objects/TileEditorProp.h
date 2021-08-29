#ifndef TILE_EDITOR_PROP_H
#define TILE_EDITOR_PROP_H
#ifndef NDEBUG

enum class TileEditorPropType { INT, FLOAT, BOOL };

struct TileEditorProp {
    const char* name;
    TileEditorPropType type;
    float value;
    float min;
    float max;
    float scale;
    bool disabled;

    TileEditorProp(const char* name, TileEditorPropType type, float value, float min, float max,
                   float scale = 1.f, bool disabled = false);

    static TileEditorProp Float(const char* name, float value, float min, float max,
                                float scale = 1.f, bool disabled = false);
    static TileEditorProp Int(const char* name, int value, int min, int max, bool disabled = false);
    static TileEditorProp Bool(const char* name, bool value, bool disabled = false);
};

#endif
#endif
