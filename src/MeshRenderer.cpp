#include "MeshRenderer.h"
#include "GameObject.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

MeshRenderer::MeshRenderer() {
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

MeshRenderer::~MeshRenderer() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
}

void MeshRenderer::render(unsigned int shaderProgram) {
    glm::mat4 model = gameObject->transform->getModelMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, glm::value_ptr(color));

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void MeshRenderer::onInspectorGUI() {
    if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::ColorEdit3("Albedo Color", glm::value_ptr(color));
    }
}