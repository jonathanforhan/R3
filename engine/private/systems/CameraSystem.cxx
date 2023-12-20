#include "systems/CameraSystem.hpp"

#include "components/CameraComponent.hpp"
#include "core/Scene.hpp"
#include "input/KeyboardEvent.hpp"
#include "input/MouseEvent.hpp"

namespace R3 {

CameraSystem::CameraSystem() {
    static bool s_initialized = false;

    if (!s_initialized) {
        Scene::bindEventListener([this](KeyPressEvent& e) {
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
            }
        });

        Scene::bindEventListener([this](KeyReleaseEvent& e) {
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
            }
        });

        Scene::bindEventListener([this](MouseButtonPressEvent& e) {
            if (e.payload.button == MouseButton::Left)
                m_mouseDown = true;
        });

        Scene::bindEventListener([this](MouseButtonReleaseEvent& e) {
            if (e.payload.button == MouseButton::Left)
                m_mouseDown = false;
        });

        Scene::bindEventListener([this](MouseCursorEvent& e) { m_cursorPosition = e.payload.cursorPosition; });

        s_initialized = true;
    }
}

void CameraSystem::tick(double dt) {
    float deltaT = static_cast<float>(dt);

    Scene::componentView<CameraComponent>().each([this, deltaT](CameraComponent& camera) {
        if (camera.active()) {
            static constexpr float mouseSensitivity = 300.0f;
            static constexpr float movementSensitivity = 4.0f;

            float deltaX = m_mouseDown ? m_cursorPosition.x - m_prevCursorPosition.x : 0.0f;
            float deltaY = m_mouseDown ? -m_cursorPosition.y + m_prevCursorPosition.y : 0.0f;
            vec2 deltaPosition = vec2(deltaX, deltaY);
            m_prevCursorPosition = m_cursorPosition;

            float deltaMovement = deltaT * movementSensitivity;

            if (m_activeKeys.w)
                camera.translateForward(deltaMovement);
            else if (m_activeKeys.s)
                camera.translateBackward(deltaMovement);

            if (m_activeKeys.a)
                camera.translateLeft(deltaMovement);
            else if (m_activeKeys.d)
                camera.translateRight(deltaMovement);

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