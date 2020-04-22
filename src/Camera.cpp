//
// Created by gioel on 22/04/2020.
//

#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) : position_(position), worldUp_(up), yaw_(yaw),
                                                                           pitch_(pitch),
                                                                           front_(glm::vec3(0.0f, 0.0f, -1.0f)),
                                                                           movementSpeed_(SPEED),
                                                                           mouseSensitivity_(SENSITIVITY), zoom_(ZOOM) {
    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 front(1.0f);
    front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front.y = sin(glm::radians(pitch_));
    front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_ = glm::normalize(front);
    right_ = glm::normalize(glm::cross(front_, worldUp_));
    up_ = glm::normalize(glm::cross(right_, front_));
}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(position_, position_ + front_, up_);
}

void Camera::ProcessKeyboard(Camera::Movement movement, float deltaTime) {
    float velocity = movementSpeed_ * deltaTime;
    if (movement == FORWARD) {
        position_ += front_ * velocity;
    }
    if (movement == BACKWARD) {
        position_ -= front_ * velocity;
    }
    if (movement == LEFT) {
        position_ -= right_ * velocity;
    }
    if (movement == RIGHT) {
        position_ += right_ * velocity;
    }
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch) {
    xOffset *= mouseSensitivity_;
    yOffset *= mouseSensitivity_;

    yaw_ += xOffset;
    pitch_ += yOffset;

    if (constrainPitch) {
        if (pitch_ > 89.9999f) {
            pitch_ = 89.9999f;
        }
        if (pitch_ < -89.9999f) {
            pitch_ = -89.9999f;
        }
    }

    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yOffset) {
    if (zoom_ >= 1.0f && zoom_ <= ZOOM) {
        zoom_ -= yOffset;
    }
    if (zoom_ < 1.0f)
        zoom_ = 1.0f;
    if (zoom_ > ZOOM)
        zoom_ = ZOOM;
}

float Camera::getZoom() const {
    return zoom_;
}

Camera::~Camera() = default;
