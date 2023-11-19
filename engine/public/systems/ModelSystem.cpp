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
        shader.writeUniform("u_Flags", flags);

        shader.writeUniform("u_ViewPosition", camera.position());

        uint32 i = 0;
        Engine::activeScene().componentView<LightComponent>().each([&i, &shader](LightComponent& light) {
            std::string name = (std::stringstream() << "u_Lights[" << char('0' + i) << ']').str();
            shader.writeUniform(name + ".position", light.position);
            shader.writeUniform(name + ".color", light.color);
            i++;
        });
        shader.writeUniform("u_NumLights", i);

        auto& renderer = Engine::renderer();

        model.mesh().bind();
        renderer.drawElements(RenderPrimitive::Triangles, model.mesh().indexCount());
    });
}

} // namespace R3
