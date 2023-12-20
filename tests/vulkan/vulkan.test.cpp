#include <R3>
#include "components/CameraComponent.hpp"
#include "components/ModelComponent.hpp"
#include "input/WindowEvent.hpp"
#include "systems/InputSystem.hpp"

using namespace R3;

class Helmet : public Entity {
public:
    void init() {
        emplace<ModelComponent>("assets/DamagedHelmet/glTF-Binary/DamagedHelmet.glb"); //
    }

    void tick(double dt) {
        auto& transform = get<TransformComponent>();
        transform = glm::rotate(transform, (float)dt, vec3(0.0f, 1.0f, 0.0f));
    }
};

void runScene() {
    Scene::bindEventListener([](const WindowCloseEvent& e) { puts("shutting down..."); });

    //--- Camera
    auto& cameraEntity = Entity::create<Entity>();
    auto& camera = cameraEntity.emplace<CameraComponent>();
    camera.setActive(true);
    camera.setPosition(vec3(0, 0, -2));

    //--- Model
    Entity::create<Helmet>();

    Engine::loop();
}

int main() {
    runScene();
    return 0;
}