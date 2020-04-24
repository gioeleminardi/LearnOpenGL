//
// Created by gioel on 22/04/2020.
//

#ifndef LEARNOPENGLCOM_CAMERA_HPP
#define LEARNOPENGLCOM_CAMERA_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>

class Camera {
public:
    constexpr static float YAW = -90.0f;
    constexpr static float PITCH = 0.0f;
    constexpr static float SPEED = 2.5f;
    constexpr static float SENSITIVITY = 0.1f;
    constexpr static float ZOOM = 45.0f;

    enum Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    explicit Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = YAW, float pitch = PITCH);

    virtual ~Camera();

    glm::mat4 getViewMatrix();

    void ProcessKeyboard(Movement movement, float deltaTime);

    void ProcessMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = true);

    void ProcessMouseScroll(float yOffset);

    float getZoom() const;

    const glm::vec3 &getPosition() const;

    const glm::vec3 &getFront() const;

private:
    void updateCameraVectors();

private:
    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_{};
    glm::vec3 right_{};
    glm::vec3 worldUp_;

    float yaw_;
    float pitch_;

    float movementSpeed_;
    float mouseSensitivity_;
    float zoom_;
};

#endif //LEARNOPENGLCOM_CAMERA_HPP
