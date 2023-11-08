#include "core/Engine.hpp"
#include "core/Entity.hpp"
#include "components/ModelComponent.hpp"
#include "components/CameraComponent.hpp"
#include "systems/CameraSystem.hpp"
#include "api/Log.hpp"
#include "api/Math.hpp"
#include "sstream"

using namespace R3;

struct Lantern : Entity {
    Lantern(Model& model)
        : model(model),
          transform(1.0f),
          position(0.0f),
          color(1.0f) {}

    void init() {
        emplace<CameraComponent>().setActive();
        Engine::activeScene().addSystem<CameraSystem>();
    }

    void tick(double dt) {
        for (auto& texture : model.textures()) {
            switch (texture.type()) {
                case TextureType::Diffuse:
                    texture.bind(0);
                    break;
                case TextureType::Specular:
                    texture.bind(1);
                    break;
                case TextureType::Emissive:
                    texture.bind(2);
                    break;
            }
        }
        model.shader().bind();

        transform = glm::scale(glm::translate(mat4(1.0f), position), vec3(0.01f));

        // model.shader().writeUniform("u_Model", transform);
        model.shader().writeUniform("u_View", Engine::activeScene().view);
        model.shader().writeUniform("u_Projection", Engine::activeScene().projection);
        model.shader().writeUniform("u_Color", color);

        model.shader().writeUniform("u_Material.diffuse", 0);
        model.shader().writeUniform("u_Material.specular", 1);
        model.shader().writeUniform("u_Material.emissive", 2);
        model.shader().writeUniform("u_Material.shininess", 1.0f);

        usize i = 0;
        Engine::activeScene().componentView<Lantern>().each([this, &i](Lantern& lantern) {
            if (i > 4)
                return;

            std::stringstream ss;
            ss << "u_Lights[" << char('0' + i) << ']';
            std::string name = ss.str();
            model.shader().writeUniform(name + ".position", lantern.position);
            model.shader().writeUniform(name + ".ambient", vec3(0.05f));
            model.shader().writeUniform(name + ".diffuse", vec3(0.8f));
            model.shader().writeUniform(name + ".specular", vec3(1.0f));
            model.shader().writeUniform(name + ".constant", 1.0f);
            model.shader().writeUniform(name + ".linear", 0.09f);
            model.shader().writeUniform(name + ".quadratic", 0.032f);
            model.shader().writeUniform(name + ".color", lantern.color);

            i++;
        });
        Engine::activeScene().componentView<CameraComponent>().each([this](CameraComponent& camera) {
            if (camera.active()) {
                model.shader().writeUniform("u_ViewPosition", camera.position());
                position = camera.position();
                transform = glm::translate(transform, 200.0f * camera.front());
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

    Model& model;
    mat4 transform;
    vec3 position;
    vec3 color;
};

struct AnyAsset : Entity {
    AnyAsset(Model& model)
        : model(model),
          transform(1.0f) {}

    void tick(double) {
        for (auto& texture : model.textures()) {
            switch (texture.type()) {
                case TextureType::Diffuse:
                    texture.bind(0);
                    break;
                case TextureType::Specular:
                    texture.bind(1);
                    break;
            }
        }
        model.shader().bind();

        model.shader().writeUniform("u_Model", transform);
        model.shader().writeUniform("u_View", Engine::activeScene().view);
        model.shader().writeUniform("u_Projection", Engine::activeScene().projection);

        model.shader().writeUniform("u_Material.diffuse", 0);
        model.shader().writeUniform("u_Material.specular", 1);
        model.shader().writeUniform("u_Material.shininess", 1.0f);

        usize i = 0;
        Engine::activeScene().componentView<Lantern>().each([this, &i](Lantern& lantern) {
            if (i > 4)
                return;

            std::stringstream ss;
            ss << "u_Lights[" << char('0' + i) << ']';
            std::string name = ss.str();
            model.shader().writeUniform(name + ".position", lantern.position);
            model.shader().writeUniform(name + ".ambient", vec3(0.05f));
            model.shader().writeUniform(name + ".diffuse", vec3(0.8f));
            model.shader().writeUniform(name + ".specular", vec3(1.0f));
            model.shader().writeUniform(name + ".constant", 1.0f);
            model.shader().writeUniform(name + ".linear", 0.09f);
            model.shader().writeUniform(name + ".quadratic", 0.032f);
            model.shader().writeUniform(name + ".color", lantern.color);

            i++;
        });
        Engine::activeScene().componentView<CameraComponent>().each([this](CameraComponent& camera) {
            if (camera.active()) {
                model.shader().writeUniform("u_ViewPosition", camera.position());
            }
        });

        for (auto& mesh : model.meshes()) {
            mesh.bind();
            Engine::renderer().drawElements(RenderPrimitive::Triangles, mesh.indexCount());
        }
    }

    Model& model;
    mat4 transform;
};

void runScene() {
    Scene& defaultScene = Engine::addScene("default", true);

    Shader lightShader(ShaderType::GLSL, "shaders/light.vert", "shaders/light.frag");
    Shader modelShader(ShaderType::GLSL, "shaders/model.vert", "shaders/model.frag");

    Model lanternModel("lantern", "scene.gltf", lightShader);
    Model backpackModel("backpack", "backpack.obj", modelShader, true);
    Model castleModel("nyc_building", "scene.gltf", modelShader);

    AnyAsset& backpack = Entity::create<AnyAsset>(&defaultScene, backpackModel);
    backpack.transform = glm::translate(backpack.transform, vec3(2.5f, 0, 0));
    backpack.transform = glm::scale(backpack.transform, vec3(0.4f));

    // lights
    Lantern& lantern = Entity::create<Lantern>(&defaultScene, lanternModel);
    lantern.color = vec3(0.97, 0.85, 0.45);

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