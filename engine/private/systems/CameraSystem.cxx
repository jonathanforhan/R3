#include "systems/CameraSystem.hpp"

#include "components/CameraComponent.hpp"
#include "core/Engine.hpp"

namespace R3 {

CameraSystem::CameraSystem() {}

void CameraSystem::tick(double dt) {
    m_deltaTime = static_cast<float>(dt);
    InputSystem::tick(dt);

#if 0
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
#endif
}

} // namespace R3