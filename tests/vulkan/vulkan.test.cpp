#include <R3>
#include "components/CameraComponent.hpp"
#include "input/KeyboardEvent.hpp"
#include "input/MouseEvent.hpp"
#include "input/WindowEvent.hpp"
#include "systems/InputSystem.hpp"

using namespace R3;

struct CameraEntity : public Entity {};

void runScene() {
    Engine::renderer()._Model = ModelComponent("assets/DamagedHelmet/glTF-Binary/DamagedHelmet.glb");

    auto& entity = Entity::create<CameraEntity>();
    auto& camera = entity.emplace<CameraComponent>();
    camera.setActive(true);

    Engine::loop();
}

int main() {
    runScene();
    return 0;
}