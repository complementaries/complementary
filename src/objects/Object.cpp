#include "Object.h"

#include "imgui/ImGuiUtils.h"
#include <imgui.h>

ObjectBase::ObjectBase()
    : hasWall(true), isStatic(true), shouldDestroy(false), destroyOnLevelLoad(true) {
}

void ObjectBase::postInit() {
}

void ObjectBase::onFaceCollision(Face playerFace) {
    (void)playerFace;
}

void ObjectBase::onCollision() {
}

bool ObjectBase::isSolid() const {
    return false;
}

bool ObjectBase::isSolidInAnyWorld() const {
    return isSolid();
}

bool ObjectBase::collidesWith(const Vector& position, const Vector& size) const {
    (void)position;
    (void)size;
    return false;
}

void ObjectBase::tick() {
}

void ObjectBase::lateTick() {
}

void ObjectBase::render(float lag) {
    (void)lag;
}

void ObjectBase::lateRender(float lag) {
    (void)lag;
}

void ObjectBase::renderText(float lag) {
    (void)lag;
}

void ObjectBase::renderEditor(float lag, bool inPalette) {
    (void)inPalette;
    render(lag);
}

#ifndef NDEBUG
const char* ObjectBase::getTypeName() {
    auto mangledName = typeid(*this).name();
    // The mangled name encodes the name length before the actual name, get rid of numbers at the
    // beginning
    while (*mangledName < 'A' && *mangledName != 0) {
        mangledName++;
    }
    return mangledName;
}

void ObjectBase::renderImGui() {
    auto propValues = new float[tileEditorProps.size()];
    for (size_t i = 0; i < tileEditorProps.size(); i++) {
        auto& prop = tileEditorProps[i];
        if (prop.disabled) {
            ImGui::PushDisabled();
        }
        switch (prop.type) {
            case TileEditorPropType::INT: {
                int val = static_cast<int>(prop.value);
                ImGui::DragInt(prop.name, &val, prop.scale, prop.min, prop.max);
                propValues[i] = static_cast<float>(val);
                break;
            }

            case TileEditorPropType::FLOAT: {
                float val = prop.value;
                ImGui::DragFloat(prop.name, &val, prop.scale, prop.min, prop.max);
                propValues[i] = val;
                break;
            }

            case TileEditorPropType::BOOL: {
                bool val = prop.value != 0.f;
                ImGui::Checkbox(prop.name, &val);
                propValues[i] = val ? 1.f : 0.f;
                break;
            }
        }
        prop.value = propValues[i];
        if (prop.disabled) {
            ImGui::PopDisabled();
        }
    }

    applyTileEditorData(propValues);
    delete[] propValues;
}
#endif

void ObjectBase::destroy() {
    shouldDestroy = true;
}

Vector ObjectBase::getSize() const {
    return Vector();
}

void ObjectBase::forceMoveParticles(const Vector& position, const Vector& size,
                                    const Vector& velocity) {
    (void)position;
    (void)size;
    (void)velocity;
}

void ObjectBase::reset() {
}

bool ObjectBase::allowSaving() const {
    return true;
}

bool ObjectBase::isKeyOfType(int type) const {
    (void)type;
    return false;
}

bool ObjectBase::isDoorOfType(int type) const {
    (void)type;
    return false;
}

void ObjectBase::addKey() {
}

bool ObjectBase::hasMoved() const {
    return true;
}