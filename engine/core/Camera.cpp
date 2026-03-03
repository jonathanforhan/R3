#include "Camera.hpp"

#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "api/Types.hpp"
#include "core/Engine.hpp"
#include "input/InputCodes.hpp"
#include "render/Window.hpp"

namespace R3 {

Camera::Camera() {
    translateBackward(2.0f);
    translateUp(1.0f);
}

void Camera::update(double dt) {
    if (!active()) {
        return;
    }

    double mouseSensitivity    = 0.1;
    double movementSensitivity = 2.5;

    if (GWindow()->keyPressed(Key::LeftShift)) {
        movementSensitivity *= 2.0;
    }

    const bool mouseDown  = GWindow()->isMouseButtonPressed(MouseButton::Left);
    const dvec2 cursorPos = GWindow()->cursorPosition();

    double deltaX, deltaY;
    if (mouseDown) {
        deltaX = +(cursorPos.x - m_prevCursorPosition.x);
        deltaY = -(cursorPos.y - m_prevCursorPosition.y);
    } else {
        deltaX = 0.0;
        deltaY = 0.0;
    }

    const dvec2 deltaPosition{deltaX, deltaY};
    m_prevCursorPosition = cursorPos;

    const double deltaMovement = dt * movementSensitivity;

    m_activeKeys.w = GWindow()->keyPressed(Key::W) ? (m_activeKeys.s + 1) : 0;
    m_activeKeys.a = GWindow()->keyPressed(Key::A) ? (m_activeKeys.d + 1) : 0;
    m_activeKeys.s = GWindow()->keyPressed(Key::S) ? (m_activeKeys.w + 1) : 0;
    m_activeKeys.d = GWindow()->keyPressed(Key::D) ? (m_activeKeys.a + 1) : 0;
    m_activeKeys.e = GWindow()->keyPressed(Key::E) ? (m_activeKeys.q + 1) : 0;
    m_activeKeys.q = GWindow()->keyPressed(Key::Q) ? (m_activeKeys.e + 1) : 0;

    if (GWindow()->uiFocused()) {
        m_activeKeys = {};
        return;
    }

    if ((m_activeKeys.w || m_activeKeys.s) && (m_activeKeys.a || m_activeKeys.d)) {
        movementSensitivity /= std::sqrt(2.0); // prevent faster diagonal movement
    }

    if (m_activeKeys.w && (m_activeKeys.w > m_activeKeys.s)) {
        translateForward(deltaMovement);
    } else if (m_activeKeys.s) {
        translateBackward(deltaMovement);
    }

    if (m_activeKeys.a && (m_activeKeys.a > m_activeKeys.d)) {
        translateLeft(deltaMovement);
    } else if (m_activeKeys.d) {
        translateRight(deltaMovement);
    }

    if (m_activeKeys.e && (m_activeKeys.e > m_activeKeys.q)) {
        translateUp(deltaMovement);
    } else if (m_activeKeys.q) {
        translateDown(deltaMovement);
    }

    if (mouseDown) {
        dvec2 dpos = deltaPosition * mouseSensitivity;
        lookAround(dpos.x, dpos.y);
    }

    switch (m_projectionMode) {
        case CameraProjectionMode::Perspective:
            applyPerspective(GWindow()->aspectRatio());
            break;
        case CameraProjectionMode::Orthographic:
            applyOrthographic(GWindow()->size());
            break;
    }
}

void Camera::translateForward(double magnitude) noexcept {
    m_position += magnitude * glm::normalize(dvec3(m_front.x, 0, m_front.z));
}

void Camera::translateBackward(double magnitude) noexcept {
    translateForward(-magnitude);
}

void Camera::translateRight(double magnitude) noexcept {
    m_position += magnitude * glm::normalize(glm::cross(m_front, m_up));
}

void Camera::translateLeft(double magnitude) noexcept {
    translateRight(-magnitude);
}

void Camera::translateUp(double magnitude) noexcept {
    m_position += m_up * magnitude;
}

void Camera::translateDown(double magnitude) noexcept {
    translateUp(-magnitude);
}

void Camera::lookAround(double dx, double dy) noexcept {
    m_yaw += dx;
    m_pitch += dy;

    m_pitch = std::clamp(m_pitch, -89.0, 89.0);

    m_front.x = std::cos(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
    m_front.y = std::sin(glm::radians(m_pitch));
    m_front.z = std::sin(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
    m_front   = glm::normalize(m_front);
}

void Camera::applyPerspective(float aspectRatio) noexcept {
    m_projection = glm::perspective(glm::radians(m_fov), aspectRatio, 0.1f, 500.0f);
    m_view       = glm::lookAt(m_position, m_position + m_front, m_up);
}

void Camera::applyOrthographic(usize2 windowSize) noexcept {
    float denom  = std::max(windowSize.x, windowSize.y) / 2.0f;
    float w      = windowSize.x / denom;
    float h      = windowSize.y / denom;
    m_projection = glm::ortho(-w, w, -h, h, -10.0f, 500.0f);
    m_view       = glm::lookAt(m_position, m_position + m_front, m_up);
}

} // namespace R3