#pragma once
#include "Component.h"
#include "Transform.h"
#include <vector>
#include <memory>
#include <string>
#include <cstring>
#include <imgui.h>

class GameObject {
public:
    std::string name;
    std::shared_ptr<Transform> transform;
    std::vector<std::shared_ptr<Component>> components;

    GameObject(const std::string& objName = "GameObject") : name(objName) {
        transform = std::make_shared<Transform>();
        transform->gameObject = this;
    }

    template<typename T, typename... Args>
    std::shared_ptr<T> addComponent(Args&&... args) {
        auto comp = std::make_shared<T>(std::forward<Args>(args)...);
        comp->gameObject = this;
        components.push_back(comp);
        return comp;
    }

    void update(float dt) {
        for (auto& comp : components) comp->update(dt);
    }

    void render(unsigned int shaderProgram) {
        for (auto& comp : components) comp->render(shaderProgram);
    }

    void onInspectorGUI() {
        char buffer[128];
        std::strncpy(buffer, name.c_str(), sizeof(buffer));
        buffer[sizeof(buffer) - 1] = '\0';
        if (ImGui::InputText("Name", buffer, sizeof(buffer))) {
            name = buffer;
        }
        ImGui::Separator();

        transform->onInspectorGUI();
        for (auto& comp : components) {
            comp->onInspectorGUI();
        }
    }
};