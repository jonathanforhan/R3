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

Camera::Camera(CameraType type)
    : m_cameraType(type) {
    translateBackward(2.0f);
    translateUp(1.0f);
}

void Camera::update(double dt) {
    if (!active()) {
        return;
    }

    float mouseSensitivity    = 0.25f;
    float movementSensitivity = 2.5f;

    if (GWindow()->keyPressed(Key::LeftShift)) {
        movementSensitivity *= 2.0f;
    }

    const bool mouseDown  = GWindow()->mouseButtonPressed(MouseButton::Left);
    const fvec2 cursorPos = GWindow()->cursorPosition();

    const float deltaX        = mouseDown ? (cursorPos.x - m_prevCursorPosition.x) : 0.0f;
    const float deltaY        = -(mouseDown ? (cursorPos.y - m_prevCursorPosition.y) : 0.0f);
    const fvec2 deltaPosition = fvec2(deltaX, deltaY);
    m_prevCursorPosition      = cursorPos;

    const float deltaMovement = (float)dt * movementSensitivity;

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
        movementSensitivity *= 0.7071f; // divide by sqrt(2) to prevent faster diagonal movement
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
        fvec2 dpos = deltaPosition * mouseSensitivity;
        lookAround(dpos.x, dpos.y);
    }
}

void Camera::translateForward(float magnitude) noexcept {
    m_position += magnitude * glm::normalize(fvec3(m_front.x, 0, m_front.z));
}

void Camera::translateBackward(float magnitude) noexcept {
    translateForward(-magnitude);
}

void Camera::translateRight(float magnitude) noexcept {
    m_position += magnitude * glm::normalize(glm::cross(m_front, m_up));
}

void Camera::translateLeft(float magnitude) noexcept {
    translateRight(-magnitude);
}

void Camera::translateUp(float magnitude) noexcept {
    m_position += m_up * magnitude;
}

void Camera::translateDown(float magnitude) noexcept {
    translateUp(-magnitude);
}

void Camera::lookAround(float dx, float dy) noexcept {
    m_yaw += dx;
    m_pitch += dy;

    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);

    m_front.x = std::cos(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
    m_front.y = std::sin(glm::radians(m_pitch));
    m_front.z = std::sin(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
    m_front   = glm::normalize(m_front);
}

void Camera::apply(float aspectRatio, ivec2 windowSize, fmat4& view, fmat4& projection) const noexcept {
    switch (m_cameraType) {
        case CameraType::Perspective: {
            projection = glm::perspective(glm::radians(m_fov), aspectRatio, 0.1f, 500.0f);
        } break;
        case CameraType::Orthographic: {
            float denom = std::max(windowSize.x, windowSize.y) / 2.0f;
            float w     = windowSize.x / denom;
            float h     = windowSize.y / denom;
            projection  = glm::ortho(-w, w, -h, h, -10.0f, 500.0f);
        } break;
        default:
            break;
    }
    view = glm::lookAt(m_position, m_position + m_front, m_up);
}

} // namespace R3