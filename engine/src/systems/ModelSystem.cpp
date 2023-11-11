#include "ModelSystem.hpp"
#include <sstream>
#include "core/Engine.hpp"
#include "components/ModelComponent.hpp"
#include "components/CameraComponent.hpp"
#include "components/LightComponent.hpp"
#include "api/Math.hpp"
#include "api/Log.hpp"

namespace R3 {

void ModelSystem::tick(double) {
    static int draws = 0;

    CameraComponent camera;
    Engine::activeScene().componentView<CameraComponent>().each([&camera](CameraComponent& cam) {
        if (cam.active()) {
            camera = cam;
        }
    });

    Engine::activeScene().componentView<ModelComponent>().each([&camera](ModelComponent& model) {
        uint32_t flags = 0;
        for (auto& texture : model.textures()) {
            uint8 t = uint8(texture.type());
            texture.bind(t);
            flags |= (1 << t);
        }

        Shader& shader = model.shader();

        shader.bind();

        shader.writeUniform("u_Model", model.transform);
        shader.writeUniform("u_View", Engine::activeScene().view);
        shader.writeUniform("u_Projection", Engine::activeScene().projection);
        shader.writeUniform("u_Tiling", model.tiling);
        shader.writeUniform("u_Flags", flags);

        shader.writeUniform("u_ViewPosition", camera.position());
        shader.writeUniform("u_Material.diffuse", 0);
        shader.writeUniform("u_Material.specular", 1);
        shader.writeUniform("u_Material.shininess", 1.0f);

        usize i = 0;
        Engine::activeScene().componentView<LightComponent>().each([&i, &shader](LightComponent& light) {
            std::string name = (std::stringstream() << "u_Lights[" << char('0' + i) << ']').str();
            shader.writeUniform(name + ".position", light.position);
            shader.writeUniform(name + ".ambient", vec3(0.05f) * light.intensity);
            shader.writeUniform(name + ".diffuse", vec3(0.8f) * 2.0f * light.intensity);
            shader.writeUniform(name + ".specular", vec3(1.0f) * light.intensity);
            shader.writeUniform(name + ".emissive", vec3(light.intensity) * 1.2f);
            shader.writeUniform(name + ".constant", 1.0f);
            shader.writeUniform(name + ".linear", 0.09f);
            shader.writeUniform(name + ".quadratic", 0.032f);
            shader.writeUniform(name + ".color", light.color);
            i++;
        });

        auto& renderer = Engine::renderer();

        model.mesh().bind();
        shader.bind();
        renderer.drawElements(RenderPrimitive::Triangles, model.mesh().indexCount());
        draws++;
    });

    LOG(Info, draws, "drawcalls");
    draws = 0;
}

} // namespace R3
