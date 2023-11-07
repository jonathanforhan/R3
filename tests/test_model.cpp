#include "core/Engine.hpp"
#include "core/Entity.hpp"
#include "components/ModelComponent.hpp"
#include "components/CameraComponent.hpp"
#include "systems/CameraSystem.hpp"

using namespace R3;

struct Player : Entity {
    Player(Model& model)
        : model(model) {}
    Model& model;

    void init() {
        emplace<CameraComponent>().setActive();
        Engine::activeScene().addSystem<CameraSystem>();
    }

    void tick(double) {
        for (uint32 i = 0; i < model.textures().size(); i++) {
            model.textures()[i].bind(i);
        }
        model.shader().bind();

        model.shader().writeUniform("u_Model", mat4(1.0f));
        model.shader().writeUniform("u_View", Engine::activeScene().view);
        model.shader().writeUniform("u_Projection", Engine::activeScene().projection);

        for (auto& mesh : model.meshes()) {
            mesh.bind();
            Engine::renderer().drawElements(RenderPrimitive::Triangles, mesh.indexCount());
        }
    }
};

int main() {
    Engine::initialize();
    Scene& defaultScene = Engine::addScene("default", true);

    Shader shader(ShaderType::GLSL, "shaders/model.vert", "shaders/model.frag");
    Model model("assets/backpack/backpack.obj", shader);
    Entity::create<Player>(&defaultScene, model);

    Engine::loop();
    return 0;
}