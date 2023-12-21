#include "systems/CameraSystem.hpp"

#include "components/CameraComponent.hpp"
#include "core/Scene.hpp"
#include "input/KeyboardEvent.hpp"
#include "input/MouseEvent.hpp"

namespace R3 {

CameraSystem::CameraSystem() {
    Scene::bindEventListener([this](const KeyPressEvent& e) {
        switch (e.payload.key) {
            case Key::W:
                m_activeKeys.w = true;
                break;
            case Key::A:
                m_activeKeys.a = true;
                break;
            case Key::S:
                m_activeKeys.s = true;
                break;
            case Key::D:
                m_activeKeys.d = true;
                break;
            case Key::E:
                m_activeKeys.e = true;
                break;
            case Key::Q:
                m_activeKeys.q = true;
                break;
            default:
                break;
        }
    });

    Scene::bindEventListener([this](const KeyReleaseEvent& e) {
        switch (e.payload.key) {
            case Key::W:
                m_activeKeys.w = false;
                break;
            case Key::A:
                m_activeKeys.a = false;
                break;
            case Key::S:
                m_activeKeys.s = false;
                break;
            case Key::D:
                m_activeKeys.d = false;
                break;
            case Key::E:
                m_activeKeys.e = false;
                break;
            case Key::Q:
                m_activeKeys.q = false;
                break;
            default:
                break;
        }
    });

    Scene::bindEventListener([this](const MouseButtonPressEvent& e) {
        if (e.payload.button == MouseButton::Left)
            m_mouseDown = true;
    });

    Scene::bindEventListener([this](const MouseButtonReleaseEvent& e) {
        if (e.payload.button == MouseButton::Left)
            m_mouseDown = false;
    });

    Scene::bindEventListener([this](const MouseCursorEvent& e) { m_cursorPosition = e.payload.cursorPosition; });
}

void CameraSystem::tick(double dt) {
    float deltaT = static_cast<float>(dt);

    Scene::componentView<CameraComponent>().each([this, deltaT](CameraComponent& camera) {
        if (camera.active()) {
            static constexpr float mouseSensitivity = 360.0f;
            static constexpr float movementSensitivity = 4.0f;

            const float deltaX = m_mouseDown ? m_cursorPosition.x - m_prevCursorPosition.x : 0.0f;
            const float deltaY = m_mouseDown ? -m_cursorPosition.y + m_prevCursorPosition.y : 0.0f;
            const vec2 deltaPosition = vec2(deltaX, deltaY);
            m_prevCursorPosition = m_cursorPosition;

            const float deltaMovement = deltaT * movementSensitivity;

            if (m_activeKeys.w)
                camera.translateForward(deltaMovement);
            else if (m_activeKeys.s)
                camera.translateBackward(deltaMovement);

            if (m_activeKeys.a)
                camera.translateLeft(deltaMovement);
            else if (m_activeKeys.d)
                camera.translateRight(deltaMovement);

            if (m_activeKeys.e)
                camera.translateUp(deltaMovement);
            else if (m_activeKeys.q)
                camera.translateDown(deltaMovement);

            if (m_mouseDown)
                camera.lookAround(deltaPosition * mouseSensitivity);

            mat4& view = Scene::view();
            mat4& projection = Scene::projection();
            camera.apply(&view, &projection, Engine::window().aspectRatio());
            Scene::setView(view);
            Scene::setProjection(projection);
        }
    });
}

} // namespace R3