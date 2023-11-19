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

struct AnyAsset : Entity {};

struct Player : Entity {};

void runScene() {
    Scene& defaultScene = Engine::addScene("default", true);

    Shader shader(ShaderType::GLSL, "shaders/pbr.vert", "shaders/pbr.frag");

    // ModelComponent helmetModel("assets/DamagedHelmet/glTF-Binary/DamagedHelmet.glb", shader);
    ModelComponent helmetModel("assets/DamagedHelmet/glTF/DamagedHelmet.gltf", shader);
    helmetModel.transform = glm::translate(helmetModel.transform, vec3(0, 0, 3));
    AnyAsset& helmet = Entity::create<AnyAsset>(&defaultScene);
    ModelComponent& helmetComponent = helmet.emplace<ModelComponent>(std::move(helmetModel));

    Player& player = Entity::create<Player>(&defaultScene);
    player.emplace<CameraComponent>().setActive();

    // lights
    LightComponent& light1 = Entity::create<AnyAsset>(&defaultScene).emplace<LightComponent>();
    light1.color = vec3(1, 0.3, 1) * 2.0f;
    light1.position = vec3(2, 2, 4);

    LightComponent& light2 = Entity::create<AnyAsset>(&defaultScene).emplace<LightComponent>();
    light2.color = vec3(0.3, 1, 1) * 2.0f;
    light2.position = vec3(-2, 2, 4);

    Engine::activeScene().addSystem<CameraSystem>();
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