#pragma once

/// @file DescriptorSetLayout.hpp
/// @brief Provides Shader Layout info for rendering

#include "render/RenderApi.hpp"

namespace R3 {

/// @brief Descriptor Set Layout Specification
struct DescriptorSetLayoutSpecification {
    const LogicalDevice& logicalDevice; ///< LogicalDevice
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