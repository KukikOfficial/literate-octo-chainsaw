#pragma once
#include "Component.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <ImGuizmo.h>

class Transform : public Component {
public:
    glm::vec3 position{0.0f};
    glm::vec3 rotation{0.0f}; // Углы Эйлера в градусах
    glm::vec3 scale{1.0f};

    glm::mat4 getModelMatrix() const {
        glm::mat4 m(1.0f);
        // Используем Recompose из ImGuizmo — это гарантирует 100% математическое
        // совпадение с декомпозицией и полностью убирает рывки при вращении
        ImGuizmo::RecomposeMatrixFromComponents(
            glm::value_ptr(position),
            glm::value_ptr(rotation),
            glm::value_ptr(scale),
            glm::value_ptr(m)
        );
        return m;
    }

    void onInspectorGUI() override {
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::DragFloat3("Position", glm::value_ptr(position), 0.05f);
            ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 0.5f);
            ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.05f, 0.001f, 50.0f);
        }
    }
};