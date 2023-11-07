#include "Cube.hpp"
#include <sstream>

void Cube::tick(double) {
    texture.bind(0);
    textureSpecular.bind(1);
    shader.bind();
    mesh.bind();

    // Vertex Shader
    shader.writeUniform("u_Model", transform);
    shader.writeUniform("u_View", Engine::activeScene().view);
    shader.writeUniform("u_Projection", Engine::activeScene().projection);

    shader.writeUniform("u_Material.diffuse", 0);
    shader.writeUniform("u_Material.specular", 1);
    shader.writeUniform("u_Material.shininess", shine);
    shader.writeUniform("u_TilingFactor", vec2(scale.x, scale.z));


    // Fragment Shader
    usize i = 0;
    Engine::activeScene().componentView<LightCube>().each([this, &i](LightCube& lightCube) {
        if (i > 4)
            return;

        std::stringstream ss;
        ss << "u_Lights[" << char('0' + i) << ']';
        std::string name = ss.str();
        shader.writeUniform(name + ".position", lightCube.position);
        shader.writeUniform(name + ".ambient", vec3(0.05f));
        shader.writeUniform(name + ".diffuse", vec3(0.8f));
        shader.writeUniform(name + ".specular", vec3(1.0f));
        shader.writeUniform(name + ".constant", 1.0f);
        shader.writeUniform(name + ".linear", 0.09f);
        shader.writeUniform(name + ".quadratic", 0.032f);
        shader.writeUniform(name + ".color", lightCube.color);

        i++;
    });
    Engine::activeScene().componentView<CameraComponent>().each([this](CameraComponent& camera) {
        if (camera.active()) {
            shader.writeUniform("u_ViewPosition", camera.position());
        }
    });

    Engine::renderer().drawArrays(RenderPrimitive::Triangles, mesh.vertexCount());
}
