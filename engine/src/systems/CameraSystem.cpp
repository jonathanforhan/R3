#include "CameraSystem.hpp"
#include "components/CameraComponent.hpp"
#include "core/Engine.hpp"

namespace R3 {

CameraSystem::CameraSystem() {
    constexpr float s = 4.0f;
    setKeyBinding(Key::Key_W, [this](InputAction) {
        Engine::activeScene().componentView<CameraComponent>().each([this](CameraComponent& camera) {
            if (camera.active())
                camera.translateForward(m_deltaTime * s);
        });
    });
    setKeyBinding(Key::Key_A, [this](InputAction) {
        Engine::activeScene().componentView<CameraComponent>().each([this](CameraComponent& camera) {
            if (camera.active())
                camera.translateLeft(m_deltaTime * s);
        });
    });
    setKeyBinding(Key::Key_S, [this](InputAction) {
        Engine::activeScene().componentView<CameraComponent>().each([this](CameraComponent& camera) {
            if (camera.active())
                camera.translateBackward(m_deltaTime * s);
        });
    });
    setKeyBinding(Key::Key_D, [this](InputAction) {
        Engine::activeScene().componentView<CameraComponent>().each([this](CameraComponent& camera) {
            if (camera.active())
                camera.translateRight(m_deltaTime * s);
        });
    });
    setKeyBinding(Key::Key_Q, [this](InputAction) {
        Engine::activeScene().componentView<CameraComponent>().each([this](CameraComponent& camera) {
            if (camera.active())
                camera.translateUp(m_deltaTime * s);
        });
    });
    setKeyBinding(Key::Key_E, [this](InputAction) {
        Engine::activeScene().componentView<CameraComponent>().each([this](CameraComponent& camera) {
            if (camera.active())
                camera.translateDown(m_deltaTime * s);
        });
    });

    setMouseBinding(MouseButton::Left, [this](InputAction action) {
        if (action == InputAction::Press) {
            m_mouseDown = true;
        } else if (action == InputAction::Release) {
            m_mouseDown = false;
        }
    });
}

void CameraSystem::tick(double dt) {
    m_deltaTime = static_cast<float>(dt);
    InputSystem::tick(dt);

    auto [xPos, yPos] = cursorPosition();
    double dx{}, dy{}, sensitivity = 500;
    if (m_mouseDown) {
        dx = xPos - m_prevCursorPosition.x;
        dy = -yPos + m_prevCursorPosition.y;
    }
    m_prevCursorPosition = dvec2(xPos, yPos);

    Engine::activeScene().componentView<CameraComponent>().each([this, dx, dy, sensitivity](CameraComponent& camera) {
        if (camera.active()) {
            Scene& scene = Engine::activeScene();
            if (m_mouseDown) {
                camera.lookAround(static_cast<float>(dx * sensitivity), static_cast<float>(dy * sensitivity));
            }
            camera.apply(&scene.view, &scene.projection, Engine::window().aspectRatio());
        }
    });
}

} // namespace R3