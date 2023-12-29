#pragma once

/// @file DescriptorSetLayout.hxx
/// @brief Provides Shader Layout info for rendering

#include <span>
#include "render/RenderApi.hxx"

namespace R3 {

struct DescriptorSetLayoutBinding {
    uint32 binding;
    DescriptorType type;
    uint32 count;
    ShaderStage::Flags stage;
};

/// @brief Descriptor Set Layout Specification
struct DescriptorSetLayoutSpecification {
    const LogicalDevice& logicalDevice;                         ///< LogicalDevice
    std::span<const DescriptorSetLayoutBinding> layoutBindings; ///< DescriptorSetLayoutBinding
};

/// @brief DescriptorSetLayout holds Shader Layout information for the binding or resources
/// Used in the allocation of DescriptorSets
class DescriptorSetLayout : public NativeRenderObject {
public:
    DEFAULT_CONSTRUCT(DescriptorSetLayout);
    NO_COPY(DescriptorSetLayout);
    DEFAULT_MOVE(DescriptorSetLayout);

    /// @brief Construct DescriptorSetLayout by spec
    /// @param spec
    DescriptorSetLayout(const DescriptorSetLayoutSpecification& spec);

    /// @brief Destroy Layout
    ~DescriptorSetLayout();

private:
    Ref<const LogicalDevice> m_logicalDevice;
};

} // namespace R3