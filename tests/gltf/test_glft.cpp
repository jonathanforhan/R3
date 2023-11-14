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

struct Lantern : Entity {};

void runScene() {
    Scene& defaultScene = Engine::addScene("default", true);
    Shader shader(ShaderType::GLSL, "shaders/default.vert", "shaders/no_shading.frag");

    ModelComponent lanternModel("lantern_gltf", "scene.gltf", shader);
    lanternModel.transform = glm::scale(mat4(1.0f), vec3(0.01f));
    Lantern& lantern = Lantern::create<Lantern>(&defaultScene);
    lantern.emplace<ModelComponent>(std::move(lanternModel));
    lantern.emplace<CameraComponent>().setActive();

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
