#include "ImGuiUtils.h"

#include <imgui.h>
#include <imgui_internal.h>

void ImGui::PushDisabled() {
    ImGuiContext& g = *GImGui;
    PushStyleVar(ImGuiStyleVar_Alpha, g.Style.Alpha * 0.6f);
    PushItemFlag(ImGuiItemFlags_Disabled, true);
}

void ImGui::PopDisabled() {
    PopItemFlag();
    PopStyleVar();
}

void ImGuiUtils::ColorPicker(const char* name, Color* color) {
    if (ImGui::CollapsingHeader(name)) {
        auto [r, g, b, a] = ColorUtils::unpackFloat(*color);
        float components[4] = {r, g, b, a};
        ImGui::ColorPicker4(name, components);
        *color = ColorUtils::rgba(components[0], components[1], components[2], components[3]);
    }
}
