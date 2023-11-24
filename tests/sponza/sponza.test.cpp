#include <R3>
#include <chrono>
#include "components/CameraComponent.hpp"
#include "components/LightComponent.hpp"
#include "components/ModelComponent.hpp"
#include "components/CubeMapComponent.hpp"
#include "systems/CameraSystem.hpp"
#include "systems/ModelSystem.hpp"

using namespace R3;

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

    AnyAsset& skybox = Entity::create<AnyAsset>(&defaultScene);
    TextureCubeMap skyboxTexture = TextureCubeMap(TextureCubeMapCreateInfo{
        .right = "textures/skybox/right.jpg",
        .left = "textures/skybox/left.jpg",
        .top = "textures/skybox/top.jpg",
        .bottom = "textures/skybox/bottom.jpg",
        .front = "textures/skybox/front.jpg",
        .back = "textures/skybox/back.jpg",
    });
    skybox.emplace<CubeMapComponent>(skyboxTexture, skyboxShader);

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