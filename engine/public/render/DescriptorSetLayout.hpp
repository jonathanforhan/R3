#pragma once

/// @brief Provides Shader Layout info for rendering

#include "render/RenderApi.hpp"

namespace R3 {

struct R3_API DescriptorSetLayoutBinding {
    uint32 binding;
    DescriptorType type;
    uint32 count;
    ShaderStage::Flags stage;
};

/// @brief Descriptor Set Layout Specification
struct R3_API DescriptorSetLayoutSpecification {
    const LogicalDevice& logicalDevice;                         ///< LogicalDevice
    std::span<const DescriptorSetLayoutBinding> layoutBindings; ///< DescriptorSetLayoutBinding
};

/// @brief DescriptorSetLayout holds Shader Layout information for the binding or resources
/// Used in the allocation of DescriptorSets
class R3_API DescriptorSetLayout : public NativeRenderObject {
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