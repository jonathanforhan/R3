#include "api/Log.hpp"
#include "api/Math.hpp"
#include "components/CameraComponent.hpp"
#include "components/LightComponent.hpp"
#include "components/ModelComponent.hpp"
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

    Player& player = Entity::create<Player>(&defaultScene);
    player.emplace<CameraComponent>().setActive();

    ModelComponent mc("assets/DamagedHelmet/glTF-Binary/DamagedHelmet.glb", shader);

    AnyAsset& aa = AnyAsset::create<AnyAsset>(&defaultScene);
    aa.emplace<ModelComponent>(std::move(mc));

    // lights
    LightComponent& light1 = Entity::create<AnyAsset>(&defaultScene).emplace<LightComponent>();
    light1.color = vec3(1, 0.3, 1) * 3.0f;
    light1.position = vec3(1, 1, 1);
    light1.intensity = 1.0f;

    LightComponent& light2 = Entity::create<AnyAsset>(&defaultScene).emplace<LightComponent>();
    light2.color = vec3(0.3, 1, 1) * 3.0f;
    light2.position = vec3(-1, 1, 1);
    light2.intensity = 1.0f;

    Engine::activeScene().addSystem<ModelSystem>();
    Engine::activeScene().addSystem<CameraSystem>();

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
