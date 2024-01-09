#include "systems/CameraSystem.hpp"

#include "components/CameraComponent.hpp"
#include "core/Entity.hpp"
#include "core/Scene.hpp"
#include "input/KeyboardEvent.hpp"
#include "input/MouseEvent.hpp"

namespace R3 {

CameraSystem::CameraSystem() {}

void CameraSystem::tick(double) {
    Entity::componentView<CameraComponent>().each([this](CameraComponent& camera) {
        if (camera.active) {
            mat4 view = Scene::view();
            mat4 projection = Scene::projection();
            camera.apply(&view, &projection, EngineInstance->window().aspectRatio());

            Scene::setView(view);
            Scene::setProjection(projection);
            Scene::setCameraPosition(camera.position());
        }
    });
}

} // namespace R3