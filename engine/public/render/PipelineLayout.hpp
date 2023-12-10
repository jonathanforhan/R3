#pragma once

#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct PipelineLayoutSpecification {
    Ref<const LogicalDevice> logicalDevice;
    Ref<const DescriptorSetLayout> descriptorSetLayout;
};

class PipelineLayout : public NativeRenderObject {
public:
    PipelineLayout() = default;
    PipelineLayout(const PipelineLayoutSpecification& spec);
    PipelineLayout(PipelineLayout&&) noexcept = default;
    PipelineLayout& operator=(PipelineLayout&&) noexcept = default;
    ~PipelineLayout();

private:
    PipelineLayoutSpecification m_spec;
};

} // namespace R3