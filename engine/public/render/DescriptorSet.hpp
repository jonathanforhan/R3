#pragma once

#include <span>
#include <vector>
#include "api/Types.hpp"
#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct DescriptorSetSpecification {
    Ref<const LogicalDevice> logicalDevice;
    Ref<const DescriptorPool> descriptorPool;
    Ref<const DescriptorSetLayout> descriptorSetLayout;
    uint32 descriptorSetCount;
};

class DescriptorSet : public NativeRenderObject {
public:
    static std::vector<DescriptorSet> allocate(const DescriptorSetSpecification& spec);
    static void free(const std::span<DescriptorSet> descriptorSets);
    void free();

    void bindUniform(const UniformBuffer& uniform, uint32 binding, usize offset = 0, usize range = 0);

private:
    DescriptorSetSpecification m_spec;
};

} // namespace R3