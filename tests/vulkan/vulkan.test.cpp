#include <R3>
#include "components/CameraComponent.hpp"
#include "components/ModelComponent.hpp"
#include "input/KeyboardEvent.hpp"

using namespace R3;

bool rotate = true;

class Helmet : public Entity {
public:
    void init() { emplace<ModelComponent>("assets/DamagedHelmet/glTF-Binary/DamagedHelmet.glb"); }

    void tick(double dt) {
        if (rotate) {
            auto& transform = get<TransformComponent>();
            transform = glm::rotate(transform, (float)dt, vec3(0.0f, 1.0f, 0.0f));
        }
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
        if (e.payload.key == Key::Space && helmet.valid())
            rotate = !rotate;
        if (e.payload.key == Key::Enter && helmet.valid())
            helmet.destroy();
    });

    Engine::loop();
}

int main() {
    try {
        runScene();
    } catch (std::exception& e) {
        LOG(Error, e.what());
    }
    return 0;
}