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
    Shader shader(ShaderType::GLSL, "shaders/default.vert", "shaders/no_shading.frag");

    Player& player = Entity::create<Player>(&defaultScene);
    player.emplace<CameraComponent>().setActive();

    ModelComponent mc("assets/Box/glTF-Binary/Box.glb", shader);
    mc.transform = glm::translate(mc.transform, vec3(0, 0, 4));

    AnyAsset& aa = AnyAsset::create<AnyAsset>(&defaultScene);
    aa.emplace<ModelComponent>(std::move(mc));

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
