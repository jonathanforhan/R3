#pragma once

#include <string_view>
#include "api/Types.hpp"
#include "render/Mesh.hpp"
#include "render/TextureBuffer.hpp"

namespace R3 {

class ModelComponent {
public:
    ModelComponent() = default;
    ModelComponent(const std::string& path);
    ModelComponent(const ModelComponent&) noexcept = delete;
    ModelComponent(ModelComponent&&) noexcept = default;
    ModelComponent& operator=(ModelComponent&&) = default;

    std::vector<Mesh>& meshes() { return m_meshes; }

private:
    std::vector<Mesh> m_meshes;
    std::vector<TextureBuffer> m_textures;
    friend class ModelLoader;
};

} // namespace R3