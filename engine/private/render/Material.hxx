#pragma once

/// @file Material.hxx

#include "render/DescriptorPool.hxx"
#include "render/RenderApi.hxx"
#include "render/TextureBuffer.hxx"
#include "render/UniformBuffer.hxx"

namespace R3 {

/// @brief Materials are POD owned by Meshes
/// Materials a DescriptorPool for binding
/// All the IDs are so we can free it later
struct Material {
    DEFAULT_CONSTRUCT(Material);
    NO_COPY(Material);
    DEFAULT_MOVE(Material);

    /// @brief Manually destroy Material, used by ModelComponent
    void destroy();

    DescriptorPool::ID descriptorPool;                    ///< Desciptor ID gotten on allocation
    PBRTextureResource textures;                          ///< PBR Texture IDs
    UniformBuffer::ID uniforms[MAX_FRAMES_IN_FLIGHT * 2]; ///< Uniform IDs
    uint32 pbrFlags = 0;                                  ///< Flags indicating pbr textures present
};

} // namespace R3