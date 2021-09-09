#ifndef IM_GUI_UTILS_H
#define IM_GUI_UTILS_H

#include "graphics/Color.h"

namespace ImGui {
    // These will be added to imgui soon, see
    // https://github.com/ocornut/imgui/issues/211#issuecomment-857704649
    void PushDisabled();
    void PopDisabled();
}

namespace ImGuiUtils {
    void ColorPicker(const char* name, Color* color);
}

#endif
