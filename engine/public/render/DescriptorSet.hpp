#pragma once

#include <vector>
#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct DescriptorSetSpecification {
    const LogicalDevice* logicalDevice;
    const DescriptorPool* descriptorPool;
    const DescriptorSetLayout* descriptorSetLayout;
    uint32 descriptorSetCount;
};

class DescriptorSet : public NativeRenderObject {
public:
    static std::vector<DescriptorSet> allocate(const DescriptorSetSpecification& spec);
    static void free(std::vector<DescriptorSet>& descriptorSets);
    void free();

    void bindUniform(const UniformBuffer& uniform, uint32 binding, usize offset = 0, usize range = 0);

private:
    DescriptorSetSpecification m_spec;
};

} // namespace R3