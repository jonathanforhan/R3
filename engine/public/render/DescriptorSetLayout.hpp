#pragma once

#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct DescriptorSetLayoutSpecification {
    const LogicalDevice* logicalDevice;
};

class DescriptorSetLayout : public NativeRenderObject {
public:
    void create(const DescriptorSetLayoutSpecification& spec);
    void destroy();

private:
    DescriptorSetLayoutSpecification m_spec;
};

} // namespace R3