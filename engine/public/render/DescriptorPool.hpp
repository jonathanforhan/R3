#pragma once

#include <span>
#include <vector>
#include "api/Types.hpp"
#include "render/DescriptorSet.hpp"
#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct DescriptorPoolSpecification {
    Ref<const LogicalDevice> logicalDevice;
    Ref<const DescriptorSetLayout> descriptorSetLayout;
    uint32 descriptorSetCount;
};

class DescriptorPool : public NativeRenderObject {
public:
    DescriptorPool() = default;
    DescriptorPool(const DescriptorPoolSpecification& spec);
    DescriptorPool(DescriptorPool&&) noexcept = default;
    DescriptorPool& operator=(DescriptorPool&&) noexcept = default;
    ~DescriptorPool();

    std::span<DescriptorSet> descriptorSets() { return m_descriptorSets; }
    DescriptorSet& descriptorSet(usize index) { return m_descriptorSets[index]; }

private:
    DescriptorPoolSpecification m_spec;

    std::vector<DescriptorSet> m_descriptorSets;
};

} // namespace R3
