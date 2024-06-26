#pragma once

/// @brief DescriptorSets provide description of how shader memory is laid out and what we can write to
/// the DescriptorSet is allocated with a layout given to it by the DescriptorPool

#include "render/StorageBuffer.hpp"

namespace R3 {

/// @brief Descriptor Set Specification
struct R3_API DescriptorSetSpecification {
    const LogicalDevice& logicalDevice;             ///< LogicalDevice
    const DescriptorPool& descriptorPool;           ///< DescriptorPool
    const DescriptorSetLayout& descriptorSetLayout; ///< DescriptorSetLayout
    uint32 descriptorSetCount;                      ///< Number of DescriptorSets to allocate
};

/// @brief Descriptor Describing Uniform Data
struct R3_API UniformDescriptor {
    const UniformBuffer& uniform;
    uint32 binding;   ///< Descriptor Shader binding
    usize offset = 0; ///< Descriptor Shader offset
    usize range = 0;  ///< Size in bytes used for update, the entire buffer is updated if range=0
};

/// @brief Descriptor Describing Storage Data
struct R3_API StorageDescriptor {
    const StorageBuffer& storageBuffer;
    uint32 binding;   ///< Descriptor Shader binding
    usize offset = 0; ///< Descriptor Shader offset
    usize range = 0;  ///< Size in bytes used for update, the entire buffer is updated if range=0
};

/// @brief Descriptor Describing Texture Data
struct R3_API TextureDescriptor {
    const TextureBuffer& texture;
    uint32 binding; ///< Descriptor Shader binding
};

/// @brief Descriptor Set Binding Specification
struct R3_API DescriptorSetBindingSpecification {
    std::span<const UniformDescriptor> uniformDescriptors; ///< UniformDescriptors
    std::span<const StorageDescriptor> storageDescriptors; ///< StorageDescriptors
    std::span<const TextureDescriptor> textureDescriptors; ///< TextureDescriptors
};

/// @brief DescriptorSets are allocated from the DescriptorPool
/// DescriptorSets contain information about the structure of Shader Bindings and are
/// bound the the CommandBuffer when we draw
class R3_API DescriptorSet : public NativeRenderObject {
public:
    DEFAULT_CONSTRUCT(DescriptorSet);
    NO_COPY(DescriptorSet);
    DEFAULT_MOVE(DescriptorSet);

    /// @brief Allocate a group of DescriptorSets, this is used by the DesciptorPool
    /// @param spec
    /// @return Descriptors
    [[nodiscard]] static std::vector<DescriptorSet> allocate(const DescriptorSetSpecification& spec);

    /// @brief Bind Shader resources to Descriptor like Uniform or Texture data
    /// @param spec
    void bindResources(const DescriptorSetBindingSpecification& spec);

private:
    Ref<const LogicalDevice> m_logicalDevice;
};

} // namespace R3
