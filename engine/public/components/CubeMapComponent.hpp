#pragma once
#include "core/Engine.hpp"
#include "render/Shader.hpp"
#include "render/TextureCubeMap.hpp"
#include "systems/CubeMapSystem.hpp"

namespace R3 {

class CubeMapComponent {
public:
    CubeMapComponent(TextureCubeMap& cubeMap, Shader& shader)
        : m_cubeMap(cubeMap),
          m_shader(shader) {
        Engine::activeScene().addSystem<CubeMapSystem>();
    }

    TextureCubeMap& texture() { return m_cubeMap; }
    Shader& shader() { return m_shader; }

private:
    TextureCubeMap& m_cubeMap;
    Shader& m_shader;
};

} // namespace R3