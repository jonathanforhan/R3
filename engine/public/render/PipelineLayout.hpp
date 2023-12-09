#pragma once

#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct PipelineLayoutSpecification {
    const LogicalDevice* logicalDevice;
    const DescriptorSetLayout* descriptorSetLayout;
};

class PipelineLayout : public NativeRenderObject {
public:
    void create(const PipelineLayoutSpecification& spec);
    void destroy();

private:
    PipelineLayoutSpecification m_spec;
};

} // namespace R3