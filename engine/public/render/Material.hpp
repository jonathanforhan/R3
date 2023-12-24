#pragma once

/// @file Material.hpp

#include <array>
#include "render/DescriptorPool.hpp"
#include "render/RenderApi.hpp"
#include "render/TextureBuffer.hpp"
#include "render/UniformBuffer.hpp"

namespace R3 {

/// @brief Materials are POD owned by Meshes
/// Materials a DescriptorPool for binding
/// All the IDs are so we can free it later
struct Material {
    /// @brief Manually destroy Material, used by ModelComponent
    void destroy();

    DescriptorPool::ID descriptorPool;                            ///< Desciptor ID gotten on allocation
    PBRTextureResource textures;                                  ///< PBR Texture IDs
    std::array<UniformBuffer::ID, MAX_FRAMES_IN_FLIGHT> uniforms; ///< Uniform IDs
};

} // namespace R3