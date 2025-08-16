#include "Camera.hpp"

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "EventHandler.hpp"
#include "Types.hpp"
#include "input/InputCodes.hpp"
#include "input/InputEvents.hpp"

namespace R3 {

Camera::Camera(CameraType type)
    : m_cameraType(type) {
    auto keyCallback = [this](const Event<KeyboardEventData>& e) {
        bool pressed = (e.id == EventHandler::id("key-press"));

        switch (e.data.key) {
            case Key::W:
                m_activeKeys.w = pressed;
                break;
            case Key::A:
                m_activeKeys.a = pressed;
                break;
            case Key::S:
                m_activeKeys.s = pressed;
                break;
            case Key::D:
                m_activeKeys.d = pressed;
                break;
            case Key::E:
                m_activeKeys.e = pressed;
                break;
            case Key::Q:
                m_activeKeys.q = pressed;
                break;
            default:
                break;
        }
    };

    EventHandler::instance().bindEventListener({"key-press", "key-release"}, keyCallback);

    auto mouseCallback = [this](const Event<MouseButtonEventData>& e) {
        bool pressed = (e.id == EventHandler::id("mouse-press"));

        if (e.data.button == MouseButton::Left) {
            m_mouseDown = pressed;
        }
    };
    EventHandler::instance().bindEventListener({"mouse-press", "mouse-release"}, mouseCallback);

    auto cursorPositionCallback = [this](const Event<MouseCursorEventData>& e) {
        m_cursorPosition = e.data.cursorPosition;
    };
    EventHandler::instance().bindEventListener("cursor-move", cursorPositionCallback);

    translateBackward(2.0f);
}

void Camera::tick(double dt) {
    float deltaT = static_cast<float>(dt);

    if (active()) {
        static constexpr float mouseSensitivity    = 360.0f;
        static constexpr float movementSensitivity = 8.0f;

        const float deltaX        = m_mouseDown ? m_cursorPosition.x - m_prevCursorPosition.x : 0.0f;
        const float deltaY        = m_mouseDown ? m_cursorPosition.y - m_prevCursorPosition.y : 0.0f;
        const fvec2 deltaPosition = fvec2(deltaX, deltaY);
        m_prevCursorPosition      = m_cursorPosition;

        const float deltaMovement = deltaT * movementSensitivity;

        if (m_activeKeys.w) {
            translateForward(deltaMovement);
        } else if (m_activeKeys.s) {
            translateBackward(deltaMovement);
        }

        if (m_activeKeys.a) {
            translateLeft(deltaMovement);
        } else if (m_activeKeys.d) {
            translateRight(deltaMovement);
        }

        if (m_activeKeys.e) {
            translateUp(deltaMovement);
        } else if (m_activeKeys.q) {
            translateDown(deltaMovement);
        }

        if (m_mouseDown) {
            fvec2 dpos = deltaPosition * mouseSensitivity;
            lookAround(dpos.x, dpos.y);
        }
    }
}

void Camera::translateForward(float magnitude) {
    m_position += magnitude * glm::normalize(fvec3(m_front.x, 0, m_front.z));
}

void Camera::translateBackward(float magnitude) {
    translateForward(-magnitude);
}

void Camera::translateRight(float magnitude) {
    m_position += magnitude * glm::normalize(glm::cross(m_front, m_up));
}

void Camera::translateLeft(float magnitude) {
    translateRight(-magnitude);
}

void Camera::translateUp(float magnitude) {
    m_position += m_up * magnitude;
}

void Camera::translateDown(float magnitude) {
    translateUp(-magnitude);
}

void Camera::lookAround(float dx, float dy) {
    m_yaw += dx;
    m_pitch += dy;

    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);

    m_front.x = std::cos(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
    m_front.y = std::sin(glm::radians(m_pitch));
    m_front.z = std::sin(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
    m_front   = glm::normalize(m_front);
}

void Camera::apply(float aspectRatio, ivec2 windowSize, fmat4& view, fmat4& projection) const {
    if (m_cameraType == CameraType::Perspective) {
        projection = glm::perspective(glm::radians(m_fov), aspectRatio, 0.1f, 500.0f);
    } else {
        float denom = std::max(windowSize.x, windowSize.y) / 2.0f;
        float w     = windowSize.x / denom;
        float h     = windowSize.y / denom;
        projection  = glm::ortho(-w, w, -h, h, -10.0f, 500.0f);
    }
    view = glm::lookAt(m_position, m_position + m_front, m_up);
}

} // namespace R3