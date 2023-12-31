#pragma once

#include "render/RenderApi.hpp"

namespace R3 {

/// @brief Pipeline Layout Specification
struct R3_API PipelineLayoutSpecification {
    const LogicalDevice& logicalDevice;             ///< LogicalDevice
    const DescriptorSetLayout& descriptorSetLayout; ///< DescriptorSetLayout
};

/// @brief Layout of Pipeline
class R3_API PipelineLayout : public NativeRenderObject {
public:
    DEFAULT_CONSTRUCT(PipelineLayout);
    NO_COPY(PipelineLayout);
    DEFAULT_MOVE(PipelineLayout);

    /// @brief Construct PipelineLayout from spec
    /// @param spec
    PipelineLayout(const PipelineLayoutSpecification& spec);

    /// @brief Destroy PipelineLayout
    ~PipelineLayout();

private:
    Ref<const LogicalDevice> m_logicalDevice;
};

} // namespace R3