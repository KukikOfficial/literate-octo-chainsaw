#pragma once
#include "Component.h"
#include <glm/glm.hpp>

class MeshRenderer : public Component {
private:
    unsigned int VAO = 0, VBO = 0;
public:
    glm::vec3 color{0.8f, 0.3f, 0.2f};

    MeshRenderer();
    ~MeshRenderer() override;

    void render(unsigned int shaderProgram) override;
    void onInspectorGUI() override;
};