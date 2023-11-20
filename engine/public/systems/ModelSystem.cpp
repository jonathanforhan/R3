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
        Shader& shader = model.shader();
        shader.bind();

        shader.writeUniform("u_Model", model.transform);
        shader.writeUniform("u_View", Engine::activeScene().view);
        shader.writeUniform("u_Projection", Engine::activeScene().projection);

        shader.writeUniform("u_ViewPosition", camera.position());

        uint32 i = 0;
        Engine::activeScene().componentView<LightComponent>().each([&i, &shader](LightComponent& light) {
            std::string name = (std::stringstream() << "u_Lights[" << i << ']').str();
            shader.writeUniform(name + ".position", light.position);
            shader.writeUniform(name + ".color", light.color);
            shader.writeUniform(name + ".intensity", vec3(light.intensity));
            i++;
        });
        shader.writeUniform("u_NumLights", i);

        for (auto& mesh : model.meshes()) {
            uint32_t flags = 0;
            for (usize iTexture : mesh.textures()) {
                Texture2D& texture = model.textures()[iTexture];
                uint8 t = texture.typeBits();
                texture.bind(t);
                flags |= (1 << t);
            }
            shader.writeUniform("u_Flags", flags);
            shader.writeUniform("u_EmissiveIntensity", model.emissiveIntensity);

            mesh.bind();

            [[likely]]
            if (mesh.indexCount()) {
                Engine::renderer().drawElements(RenderPrimitive::Triangles, mesh.indexCount());
            } else {
                Engine::renderer().drawArrays(RenderPrimitive::Triangles, mesh.vertexCount());
            }
        }
    });
}

} // namespace R3
