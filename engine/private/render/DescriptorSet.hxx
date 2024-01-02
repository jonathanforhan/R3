#pragma once

/// @brief DescriptorSets provide description of how shader memory is laid out and what we can write to
/// the DescriptorSet is allocated with a layout given to it by the DescriptorPool

#include <span>
#include <vector>
#include "render/RenderApi.hxx"
#include "render/StorageBuffer.hxx"
#include "render/TextureBuffer.hxx"
#include "render/UniformBuffer.hxx"

namespace R3 {

/// @brief Descriptor Set Specification
struct DescriptorSetSpecification {
    const LogicalDevice& logicalDevice;             ///< LogicalDevice
    const DescriptorPool& descriptorPool;           ///< DescriptorPool
    const DescriptorSetLayout& descriptorSetLayout; ///< DescriptorSetLayout
    uint32 descriptorSetCount;                      ///< Number of DescriptorSets to allocate
};

/// @brief Descriptor Describing Uniform Data
struct UniformDescriptor {
    UniformBuffer::ID uniform;
    uint32 binding; ///< Descriptor Shader binding
    usize offset;   ///< Descriptor Shader offset
    usize range;    ///< Size in bytes used for update, the entire buffer is updated if range=0
};

/// @brief Descriptor Describing Storage Data
struct StorageDescriptor {
    const StorageBuffer& storageBuffer; // TODO match other API
    uint32 binding;                     ///< Descriptor Shader binding
    usize offset;                       ///< Descriptor Shader offset
    usize range;                        ///< Size in bytes used for update, the entire buffer is updated if range=0
};

/// @brief Descriptor Describing Texture Data
struct TextureDescriptor {
    TextureBuffer::ID texture;
    uint32 binding; ///< Descriptor Shader binding
};

/// @brief Descriptor Set Binding Specification
struct DescriptorSetBindingSpecification {
    std::span<const UniformDescriptor> uniformDescriptors; ///< UniformDescriptors
    std::span<const StorageDescriptor> storageDescriptors; ///< StorageDescriptors
    std::span<const TextureDescriptor> textureDescriptors; ///< TextureDescriptors
};

/// @brief DescriptorSets are allocated from the DescriptorPool
/// DescriptorSets contain information about the structure of Shader Bindings and are
/// bound the the CommandBuffer when we draw
class DescriptorSet : public NativeRenderObject {
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