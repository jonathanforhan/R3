#include <chrono>
#include <sstream>
#include "api/Log.hpp"
#include "api/Math.hpp"
#include "components/CameraComponent.hpp"
#include "components/ModelComponent.hpp"
#include "core/Engine.hpp"
#include "core/Entity.hpp"
#include "systems/CameraSystem.hpp"
#include "systems/ModelSystem.hpp"

using namespace R3;

struct Lantern : Entity {
    Lantern(ModelComponent& model)
        : model(model),
          transform(1.0f),
          position(0.0f),
          color(1.0f) {}

    void init() {
        emplace<CameraComponent>().setActive();
        Engine::activeScene().addSystem<CameraSystem>();
    }

    void tick(double dt) {
        if (shouldDim && intensity > 0.0f) {
            intensity -= static_cast<float>(dt);
            if (intensity < 0.0f) {
                intensity = 0.0f;
                shouldDim = false;
            }
        }

        uint32_t flags = 0;
        for (auto& texture : model.textures()) {
            uint32 t = uint32(texture.type());
            texture.bind(t);
            flags |= (1 << t);
        }
        model.shader().bind();

        model.shader().writeUniform("u_View", Engine::activeScene().view);
        model.shader().writeUniform("u_Projection", Engine::activeScene().projection);
        model.shader().writeUniform("u_Tiling", 1.0f);
        model.shader().writeUniform("u_Flags", flags);

        model.shader().writeUniform("u_Material.diffuse", TextureTypeBits::Diffuse);
        model.shader().writeUniform("u_Material.specular", TextureTypeBits::Specular);
        model.shader().writeUniform("u_Material.emissive", TextureTypeBits::Emissive);
        model.shader().writeUniform("u_Material.shininess", 1.0f);

        usize i = 0;
        Engine::activeScene().componentView<Lantern>().each([this, &i](Lantern& lantern) {
            std::string name = (std::stringstream() << "u_Lights[" << char('0' + i) << ']').str();
            model.shader().writeUniform(name + ".position", lantern.position);
            model.shader().writeUniform(name + ".ambient", vec3(0.05f));
            model.shader().writeUniform(name + ".diffuse", vec3(0.8f));
            model.shader().writeUniform(name + ".specular", vec3(1.0f));
            model.shader().writeUniform(name + ".emissive", vec3(lantern.intensity) * (float)pow(lantern.intensity, 2));
            model.shader().writeUniform(name + ".constant", 1.0f / lantern.intensity);
            model.shader().writeUniform(name + ".linear", 0.09f / lantern.intensity);
            model.shader().writeUniform(name + ".quadratic", 0.032f / float(pow(lantern.intensity, 3)));
            model.shader().writeUniform(name + ".color", lantern.color);

            i++;
        });
        Engine::activeScene().componentView<CameraComponent>().each([this](CameraComponent& camera) {
            if (camera.active()) {
                model.shader().writeUniform("u_ViewPosition", camera.position());
                position = camera.position();
                transform = glm::scale(glm::translate(mat4(1.0f), position), vec3(0.005f));
                transform = glm::translate(transform, vec3(200.0f) * camera.front());
                transform = glm::translate(transform, vec3(0, -40, 0));
                vec3 side = glm::normalize(glm::cross(camera.front(), vec3(0, 1, 0)));
                transform = glm::translate(transform, 100.0f * side);
            }
        });

        model.shader().writeUniform("u_Model", transform);

        for (auto& mesh : model.meshes()) {
            mesh.bind();
            Engine::renderer().drawElements(RenderPrimitive::Triangles, mesh.indexCount());
        }
    }

    ModelComponent& model;
    mat4 transform;
    vec3 position;
    vec3 color;
    float intensity = 1.0f;
    bool shouldDim = false;
};

std::chrono::system_clock::time_point prev = std::chrono::system_clock::now();

struct LanternSystem : InputSystem {
    LanternSystem() {
        setKeyBinding(Key::Key_H, [](InputAction action) {
            Engine::activeScene().componentView<Lantern>().each([action](Lantern& lantern) {
                if (action == InputAction::Press) {
                    auto now = std::chrono::system_clock::now();
                    if (std::chrono::duration<double>(now - prev).count() > 0.2) {
                        if (lantern.intensity && !lantern.shouldDim) {
                            lantern.shouldDim = true;
                        } else {
                            lantern.intensity = 1.0f;
                            lantern.shouldDim = false;
                        }
                        prev = now;
                    }
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
    floorComponent.tiling = vec2(10.0f);

    // lights
    Lantern& lantern = Entity::create<Lantern>(&defaultScene, lanternModel);
    lantern.color = vec3(0.97, 0.85, 0.45);

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