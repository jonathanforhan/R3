#include <chrono>
#include "api/Log.hpp"
#include "api/Math.hpp"
#include "components/CameraComponent.hpp"
#include "components/ModelComponent.hpp"
#include "components/LightComponent.hpp"
#include "core/Engine.hpp"
#include "core/Entity.hpp"
#include "systems/CameraSystem.hpp"
#include "systems/ModelSystem.hpp"

using namespace R3;

struct Lantern : Entity {
    Lantern() = default;

    void init() {
        LightComponent& light = emplace<LightComponent>();
        light.color = vec3(0.97, 0.85, 0.45);
        light.intensity = 1.0f;

        emplace<CameraComponent>().setActive();
        Engine::activeScene().addSystem<CameraSystem>();
    }

    void tick(double dt) {
        LightComponent& light = get<LightComponent>();
        vec3 position{};

        Engine::activeScene().componentView<CameraComponent>().each([&](CameraComponent& camera) {
            vec3 side = 0.5f * glm::normalize(glm::cross(camera.front(), vec3(0, 1, 0)));
            vec3 down = -vec3(0, 0.3, 0);
            position = camera.position() + camera.front() + side + down;
                        
        });

        ModelComponent& lamp = get<ModelComponent>();
        lamp.transform = glm::scale(glm::translate(mat4(1.0f), position), vec3(0.005f));
        light.position = position;

        if (shouldDim && light.intensity > 0.0f) {
            light.intensity -= static_cast<float>(dt);
            if (light.intensity < 0.0f) {
                light.intensity = 0.0f;
                shouldDim = false;
            }
        }
    }

    bool shouldDim = false;
};

std::chrono::system_clock::time_point prev = std::chrono::system_clock::now();

struct LanternSystem : InputSystem {
    LanternSystem() {
        setKeyBinding(Key::Key_H, [](InputAction action) {
            Engine::activeScene().componentView<Lantern>().each([action](Lantern& lantern) {
                if (action != InputAction::Press) {
                    return;
                }
                auto now = std::chrono::system_clock::now();
                if (std::chrono::duration<double>(now - prev).count() > 0.2) {
                    LightComponent& light = lantern.get<LightComponent>();
                    if (light.intensity && !lantern.shouldDim) {
                        lantern.shouldDim = true;
                    } else {
                        light.intensity = 1.0f;
                        lantern.shouldDim = false;
                    }
                    prev = now;
                }
            });
        });
    }
};

struct AnyAsset : Entity {};

void runScene() {
    Scene& defaultScene = Engine::addScene("default", true);

    Shader shader(ShaderType::GLSL, "shaders/default.vert", "shaders/default.frag");

    ModelComponent lanternModel("lantern", "scene.gltf", shader);
    ModelComponent backpackModel("backpack", "backpack.obj", shader, true);
    ModelComponent floorModel("floor", "scene.gltf", shader);

    AnyAsset& backpack = Entity::create<AnyAsset>(&defaultScene);
    ModelComponent& backpackComponent = backpack.emplace<ModelComponent>(std::move(backpackModel));
    backpackComponent.transform = glm::translate(backpackComponent.transform, vec3(2.5f, 0, 0));
    backpackComponent.transform = glm::scale(backpackComponent.transform, vec3(0.4f));

    AnyAsset& floor = Entity::create<AnyAsset>(&defaultScene);
    ModelComponent& floorComponent = floor.emplace<ModelComponent>(std::move(floorModel));
    floorComponent.transform = glm::translate(floorComponent.transform, vec3(0, -1, 0));
    floorComponent.transform = glm::scale(floorComponent.transform, vec3(0.1f));
    floorComponent.tiling = vec2(15.0f);

    // lights
    Lantern& lantern = Entity::create<Lantern>(&defaultScene);
    lantern.emplace<ModelComponent>(std::move(lanternModel));

    Engine::activeScene().addSystem<LanternSystem>();
    Engine::activeScene().addSystem<ModelSystem>();

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