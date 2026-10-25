#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    glm::vec3 position{0.0f, 2.0f, 5.0f};
    glm::vec3 front{0.0f, 0.0f, -1.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};
    glm::vec3 right{1.0f, 0.0f, 0.0f};

    float yaw = -90.0f;
    float pitch = -15.0f;
    float speed = 4.0f;
    float sensitivity = 0.1f;

    Camera();
    glm::mat4 getViewMatrix() const;
    void processKeyboard(int direction, float dt);
    void processMouse(float xoffset, float yoffset);

private:
    void updateVectors();
};