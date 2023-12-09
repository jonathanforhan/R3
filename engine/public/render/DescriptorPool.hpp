#pragma once

#include <vector>
#include "render/DescriptorSet.hpp"
#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct DescriptorPoolSpecification {
    const LogicalDevice* logicalDevice;
    const DescriptorSetLayout* descriptorSetLayout;
    uint32 descriptorSetCount;
};

class DescriptorPool : public NativeRenderObject {
public:
    void create(const DescriptorPoolSpecification& spec);
    void destroy();

    std::vector<DescriptorSet>& descriptorSets() { return m_descriptorSets; }
    DescriptorSet& descriptorSet(usize index) { return m_descriptorSets[index]; }

private:
    DescriptorPoolSpecification m_spec;

    std::vector<DescriptorSet> m_descriptorSets;
};

} // namespace R3
