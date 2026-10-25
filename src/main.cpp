#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuizmo.h>

#include "Shader.h"
#include "Camera.h"
#include "GameObject.h"
#include "MeshRenderer.h"

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

const char* vShader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

const char* fShader = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;

void main() {
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
)";

Camera mainCamera;
bool isCameraActive = false;
bool firstMouse = true;
float lastX = 400.0f, lastY = 300.0f;

ImGuizmo::OPERATION currentGizmoOperation = ImGuizmo::TRANSLATE;
ImGuizmo::MODE currentGizmoMode = ImGuizmo::LOCAL;

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    if (!isCameraActive) return;

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    mainCamera.processMouse(xoffset, yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            isCameraActive = true;
            firstMouse = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else if (action == GLFW_RELEASE) {
            isCameraActive = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

void processInput(GLFWwindow* window, float dt) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (isCameraActive) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) mainCamera.processKeyboard(0, dt);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) mainCamera.processKeyboard(1, dt);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) mainCamera.processKeyboard(2, dt);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) mainCamera.processKeyboard(3, dt);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) mainCamera.processKeyboard(4, dt);
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) mainCamera.processKeyboard(5, dt);
    } else {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) currentGizmoOperation = ImGuizmo::TRANSLATE;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) currentGizmoOperation = ImGuizmo::ROTATE;
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) currentGizmoOperation = ImGuizmo::SCALE;
    }
}

int main() {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int scrWidth = 1280;
    int scrHeight = 720;
    GLFWwindow* window = glfwCreateWindow(scrWidth, scrHeight, "MiniEngine - Inspector & Gizmo", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    Shader shader(vShader, fShader);

    std::vector<std::shared_ptr<GameObject>> scene;

    auto redCube = std::make_shared<GameObject>("Red Cube");
    redCube->transform->position = glm::vec3(-1.5f, 0.0f, 0.0f);
    auto mesh1 = redCube->addComponent<MeshRenderer>();
    mesh1->color = glm::vec3(0.9f, 0.2f, 0.2f);
    scene.push_back(redCube);

    auto greenCube = std::make_shared<GameObject>("Green Cube");
    greenCube->transform->position = glm::vec3(1.5f, 0.0f, 0.0f);
    auto mesh2 = greenCube->addComponent<MeshRenderer>();
    mesh2->color = glm::vec3(0.2f, 0.8f, 0.3f);
    scene.push_back(greenCube);

    auto floor = std::make_shared<GameObject>("Floor");
    floor->transform->position = glm::vec3(0.0f, -1.0f, 0.0f);
    floor->transform->scale = glm::vec3(10.0f, 0.2f, 10.0f);
    auto floorMesh = floor->addComponent<MeshRenderer>();
    floorMesh->color = glm::vec3(0.25f, 0.3f, 0.35f);
    scene.push_back(floor);

    std::shared_ptr<GameObject> selectedObject = redCube;
    int objectCounter = 1;

    glm::vec3 lightPos(3.0f, 5.0f, 3.0f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        processInput(window, deltaTime);

        // Удаление выбранного объекта клавишей Delete
        if (selectedObject && !isCameraActive && ImGui::IsKeyPressed(ImGuiKey_Delete)) {
            auto it = std::find(scene.begin(), scene.end(), selectedObject);
            if (it != scene.end()) {
                scene.erase(it);
                selectedObject = nullptr;
            }
        }

        glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
        glViewport(0, 0, scrWidth, scrHeight);

        for (auto& obj : scene) obj->update(deltaTime);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();

        // --- 1. ПАНЕЛЬ ИНСТРУМЕНТОВ ---
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::Begin("Tools", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        if (ImGui::RadioButton("Translate (W)", currentGizmoOperation == ImGuizmo::TRANSLATE))
            currentGizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate (E)", currentGizmoOperation == ImGuizmo::ROTATE))
            currentGizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale (R)", currentGizmoOperation == ImGuizmo::SCALE))
            currentGizmoOperation = ImGuizmo::SCALE;

        ImGui::Separator();
        if (ImGui::RadioButton("Local", currentGizmoMode == ImGuizmo::LOCAL))
            currentGizmoMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", currentGizmoMode == ImGuizmo::WORLD))
            currentGizmoMode = ImGuizmo::WORLD;
        ImGui::End();

        // --- 2. ОКНО HIERARCHY (С СОЗДАНИЕМ И УДАЛЕНИЕМ ОБЪЕКТОВ) ---
        ImGui::SetNextWindowPos(ImVec2(10, 110), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(240, 320), ImGuiCond_FirstUseEver);
        ImGui::Begin("Hierarchy");

        // Кнопка создания нового куба
        if (ImGui::Button("+ Create Cube", ImVec2(-1, 26))) {
            auto newObj = std::make_shared<GameObject>("Cube " + std::to_string(objectCounter++));
            newObj->transform->position = glm::vec3(0.0f, 1.0f, 0.0f);
            auto mesh = newObj->addComponent<MeshRenderer>();
            mesh->color = glm::vec3(0.4f, 0.6f, 0.9f);
            scene.push_back(newObj);
            selectedObject = newObj;
        }

        ImGui::Separator();

        int toDeleteIndex = -1;
        for (int i = 0; i < static_cast<int>(scene.size()); ++i) {
            auto& obj = scene[i];
            bool isSelected = (selectedObject == obj);

            ImGui::PushID(i);
            if (ImGui::Selectable(obj->name.c_str(), isSelected)) {
                selectedObject = obj;
            }

            // Контекстное меню по клику правой кнопкой мыши по элементу
            if (ImGui::BeginPopupContextItem()) {
                selectedObject = obj;
                if (ImGui::MenuItem("Delete")) {
                    toDeleteIndex = i;
                }
                ImGui::EndPopup();
            }
            ImGui::PopID();
        }

        if (toDeleteIndex != -1) {
            if (selectedObject == scene[toDeleteIndex]) {
                selectedObject = nullptr;
            }
            scene.erase(scene.begin() + toDeleteIndex);
        }
        ImGui::End();

        // --- 3. ОКНО INSPECTOR ---
        ImGui::SetNextWindowPos(ImVec2((float)scrWidth - 300, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(290, 380), ImGuiCond_FirstUseEver);
        ImGui::Begin("Inspector");
        if (selectedObject) {
            selectedObject->onInspectorGUI();
            ImGui::Separator();
            if (ImGui::Button("Delete Object", ImVec2(-1, 0))) {
                auto it = std::find(scene.begin(), scene.end(), selectedObject);
                if (it != scene.end()) {
                    scene.erase(it);
                    selectedObject = nullptr;
                }
            }
        } else {
            ImGui::TextDisabled("Select an object in Hierarchy");
        }
        ImGui::End();

        // Отрисовка OpenGL сцены
        glClearColor(0.12f, 0.13f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)scrWidth / (float)scrHeight, 0.1f, 100.0f);
        glm::mat4 view = mainCamera.getViewMatrix();

        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setVec3("lightPos", lightPos);
        shader.setVec3("lightColor", lightColor);
        shader.setVec3("viewPos", mainCamera.position);

        for (auto& obj : scene) obj->render(shader.id);

        // --- 4. ОТРИСОВКА И ПРИМЕНЕНИЕ GIZMO ---
        if (selectedObject) {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
            ImGuizmo::SetRect(0, 0, (float)scrWidth, (float)scrHeight);

            glm::mat4 model = selectedObject->transform->getModelMatrix();

            ImGuizmo::Manipulate(
                glm::value_ptr(view),
                glm::value_ptr(projection),
                currentGizmoOperation,
                currentGizmoMode,
                glm::value_ptr(model)
            );

            if (ImGuizmo::IsUsing()) {
                float matrixTranslation[3], matrixRotation[3], matrixScale[3];
                ImGuizmo::DecomposeMatrixToComponents(
                    glm::value_ptr(model),
                    matrixTranslation,
                    matrixRotation,
                    matrixScale
                );

                // Изолируем применение параметров в зависимости от текущей операции
                if (currentGizmoOperation == ImGuizmo::TRANSLATE) {
                    selectedObject->transform->position = glm::make_vec3(matrixTranslation);
                } else if (currentGizmoOperation == ImGuizmo::ROTATE) {
                    selectedObject->transform->rotation = glm::make_vec3(matrixRotation);
                } else if (currentGizmoOperation == ImGuizmo::SCALE) {
                    selectedObject->transform->scale = glm::make_vec3(matrixScale);
                }
            }
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    return 0;
}