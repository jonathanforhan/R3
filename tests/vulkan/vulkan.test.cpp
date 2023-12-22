#include <R3>
#include "components/CameraComponent.hpp"
#include "components/ModelComponent.hpp"
#include "input/KeyboardEvent.hpp"

#define R3_TEST_TRY 1

using namespace R3;

class Helmet : public Entity {
public:
    void init() { emplace<ModelComponent>("assets/DamagedHelmet/glTF-Binary/DamagedHelmet.glb"); }

    void tick(double dt) {
        auto& transform = get<TransformComponent>();
        transform = glm::rotate(transform, (float)dt, vec3(0.0f, 1.0f, 0.0f));
    }
};

void runScene() {
    //--- Camera
    auto& cameraEntity = Entity::create<Entity>();
    auto& camera = cameraEntity.emplace<CameraComponent>();
    camera.setActive(true);
    camera.setPosition(vec3(0, 0, -2));

    //--- Model
    auto& helmet = Entity::create<Helmet>();

    auto& entity = Entity::create<Entity>();
    entity.emplace<ModelComponent>("assets/Sponza/glTF/Sponza.gltf");
    auto& transform = entity.get<TransformComponent>();
    transform = glm::translate(transform, vec3(0, -2, 0));

    Scene::bindEventListener([&](const KeyPressEvent& e) {
        if (e.payload.key == Key::Space)
            helmet.destroy();
        else if (e.payload.key == Key::Enter)
            entity.destroy();
    });

    Engine::loop();
}

int main() {
#if R3_TEST_TRY
    try {
#endif
        runScene();
#if R3_TEST_TRY
    } catch (std::exception& e) {
        LOG(Error, e.what());
    }
#endif
    return 0;
}