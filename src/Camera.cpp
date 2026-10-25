#include "Camera.h"
#include <cmath>

Camera::Camera() {
    updateVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

void Camera::processKeyboard(int direction, float dt) {
    float velocity = speed * dt;
    if (direction == 0) position += front * velocity; // W
    if (direction == 1) position -= front * velocity; // S
    if (direction == 2) position -= right * velocity; // A
    if (direction == 3) position += right * velocity; // D
    if (direction == 4) position += up * velocity;    // Space
    if (direction == 5) position -= up * velocity;    // Ctrl
}

void Camera::processMouse(float xoffset, float yoffset) {
    yaw += xoffset * sensitivity;
    pitch += yoffset * sensitivity;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    updateVectors();
}

void Camera::updateVectors() {
    glm::vec3 f;
    f.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    f.y = std::sin(glm::radians(pitch));
    f.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    front = glm::normalize(f);
    right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
    up    = glm::normalize(glm::cross(right, front));
}