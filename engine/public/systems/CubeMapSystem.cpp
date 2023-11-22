#include "CubeMapSystem.hpp"
#include "components/CubeMapComponent.hpp"
#include "core/BasicGeometry.hpp"
#include "core/Engine.hpp"
#include "api/Log.hpp"

namespace R3 {

CubeMapSystem::CubeMapSystem() {
    Vertex vertices[36];
    Cube(vertices);
    m_cubeMesh = Mesh(vertices);
}

void CubeMapSystem::tick(double dt) {
    Engine::renderer().depthFunction(DepthFunction::LessEqual);
    m_cubeMesh.bind();

    Engine::activeScene().componentView<CubeMapComponent>().each([](CubeMapComponent& cubeMap) {
        cubeMap.texture().bind(0);
        cubeMap.shader().bind();
        cubeMap.shader().writeUniform("u_Skybox", 0);
        cubeMap.shader().writeUniform("u_View", mat4(mat3(Engine::activeScene().view)));
        cubeMap.shader().writeUniform("u_Projection", Engine::activeScene().projection);
        Engine::renderer().drawArrays(RenderPrimitive::Triangles, 36);
    });

    Engine::renderer().depthFunction(DepthFunction::Less);
}

} // namespace R3