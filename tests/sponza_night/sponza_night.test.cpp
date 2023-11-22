#include <R3>
#include <chrono>
#include "components/CameraComponent.hpp"
#include "components/LightComponent.hpp"
#include "components/ModelComponent.hpp"
#include "components/CubeMapComponent.hpp"
#include "systems/CameraSystem.hpp"
#include "systems/ModelSystem.hpp"

using namespace R3;

struct Lantern : Entity {
    Lantern() = default;

    void init() {
        LightComponent& light = emplace<LightComponent>();
        light.color = vec3(0.97, 0.85, 0.45) * 2.0f;
        light.intensity = 1.0f;
    }

    void tick(double dt) {
        LightComponent& light = get<LightComponent>();
        vec3 position{};

        Engine::activeScene().componentView<CameraComponent>().each([&](auto& camera) {
            vec3 side = 0.5f * glm::normalize(glm::cross(camera.front(), vec3(0, 1, 0)));
            vec3 down = -vec3(0, 0.3, 0);
            position = camera.position() + camera.front() + side + down;
        });

        ModelComponent& lamp = get<ModelComponent>();
        lamp.transform = glm::scale(glm::translate(mat4(1.0f), position), vec3(0.005f));
        lamp.emissiveIntensity = light.intensity;
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
            if (action != InputAction::Press) {
                return;
            }
            Engine::activeScene().componentView<Lantern>().each([action](Lantern& lantern) {
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
        setKeyBinding(Key::Key_K, [](InputAction action) {
            if (action != InputAction::Press) {
                return;
            }
            auto now = std::chrono::system_clock::now();
            Engine::activeScene().componentView<LightComponent>().each([now, action](LightComponent& light) {
                if (std::chrono::duration<double>(now - prev).count() > 0.2) {
                    if (light.position.y == 2.0f) {
                        if (light.intensity == 0.0f) {
                            light.intensity = 1.0f;
                        } else {
                            light.intensity = 0.0f;
                        }
                    }
                }
            });
            prev = now;
        });
    }
};

struct AnyAsset : Entity {};

void runScene() {
    Scene& defaultScene = Engine::addScene("default", true);

    Shader shader(ShaderType::GLSL, "shaders/pbr.vert", "shaders/pbr.frag");
    Shader skyboxShader(ShaderType::GLSL, "shaders/cubemap.vert", "shaders/cubemap.frag");

    AnyAsset& sponza = Entity::create<AnyAsset>(&defaultScene);
    ModelComponent& sponzaComponent = sponza.emplace<ModelComponent>("assets/Sponza/glTF/Sponza.gltf", shader);

    AnyAsset& player = Entity::create<AnyAsset>(&defaultScene);
    CameraComponent& camera = player.emplace<CameraComponent>(CameraType::Perspective);
    camera.setActive();
    camera.setPosition(vec3(0, 2, 0));

    Lantern& lantern = Entity::create<Lantern>(&defaultScene);
    ModelComponent& laternComponent = lantern.emplace<ModelComponent>("assets/Lantern/Lantern.glb", shader);

    // lights
    for (int i = -4; i <= 4; i += 2) {
        LightComponent& light1 = Entity::create<AnyAsset>(&defaultScene).emplace<LightComponent>();
        light1.color = vec3(0.97, 0.84, 0.92);
        light1.position = vec3(i, 2, 1);
        light1.intensity = 0.0f;

        LightComponent& light2 = Entity::create<AnyAsset>(&defaultScene).emplace<LightComponent>();
        light2.color = vec3(0.97, 0.84, 0.92);
        light2.position = vec3(i, 2, -1);
        light2.intensity = 0.0f;
    }

    Engine::activeScene().addSystem<LanternSystem>();

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