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
