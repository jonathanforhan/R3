#include "ModelSystem.hpp"
#include <sstream>
#include "core/Engine.hpp"
#include "components/ModelComponent.hpp"
#include "components/CameraComponent.hpp"
#include "components/LightComponent.hpp"

namespace R3 {

void ModelSystem::tick(double) {
    Engine::activeScene().componentView<ModelComponent>().each([](ModelComponent& model) {
        uint32_t flags = 0;
        for (auto& texture : model.textures()) {
            uint32 t = uint32(texture.type());
            texture.bind(t);
            flags |= (1 << t);
        }

        model.shader().bind();

        model.shader().writeUniform("u_Model", model.transform);
        model.shader().writeUniform("u_View", Engine::activeScene().view);
        model.shader().writeUniform("u_Projection", Engine::activeScene().projection);
        model.shader().writeUniform("u_Tiling", model.tiling);
        model.shader().writeUniform("u_Flags", flags);

        model.shader().writeUniform("u_Material.diffuse", 0);
        model.shader().writeUniform("u_Material.specular", 1);
        model.shader().writeUniform("u_Material.shininess", 1.0f);

        usize i = 0;
        Engine::activeScene().componentView<LightComponent>().each([&i, &model](LightComponent& light) {
            std::string name = (std::stringstream() << "u_Lights[" << char('0' + i) << ']').str();
            model.shader().writeUniform(name + ".position", light.position);
            model.shader().writeUniform(name + ".ambient", vec3(0.05f));
            model.shader().writeUniform(name + ".diffuse", vec3(0.8f) * 3.0f);
            model.shader().writeUniform(name + ".specular", vec3(1.0f));
            model.shader().writeUniform(name + ".constant", 1.0f / light.intensity);
            model.shader().writeUniform(name + ".linear", 0.09f / light.intensity);
            model.shader().writeUniform(name + ".quadratic", 0.032f / float(pow(light.intensity, 3)));
            model.shader().writeUniform(name + ".color", light.color);
            i++;
        });

        Engine::activeScene().componentView<CameraComponent>().each([&model](CameraComponent& camera) {
            if (camera.active()) {
                model.shader().writeUniform("u_ViewPosition", camera.position());
            }
        });

        for (auto& mesh : model.meshes()) {
            mesh.bind();
            Engine::renderer().drawElements(RenderPrimitive::Triangles, mesh.indexCount());
        }
    });
}

} // namespace R3
