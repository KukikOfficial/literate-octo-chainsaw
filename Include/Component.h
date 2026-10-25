#pragma once

class GameObject;

class Component {
public:
    GameObject* gameObject = nullptr;
    virtual ~Component() = default;

    virtual void update(float dt) {}
    virtual void render(unsigned int shaderProgram) {}
    virtual void onInspectorGUI() {}
};