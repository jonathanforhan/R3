#pragma once

#include <vector>
#include "api/Types.hpp"
#include "render/DescriptorSet.hpp"
#include "render/DescriptorSetLayout.hpp"
#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct DescriptorPoolSpecification {
    Ref<const LogicalDevice> logicalDevice;
    uint32 descriptorSetCount;
};

class DescriptorPool : public NativeRenderObject {
public:
    DescriptorPool() = default;
    DescriptorPool(const DescriptorPoolSpecification& spec);
    DescriptorPool(DescriptorPool&&) noexcept = default;
    DescriptorPool& operator=(DescriptorPool&&) noexcept = default;
    ~DescriptorPool();

    const DescriptorSetLayout& layout() const { return m_layout; }
    std::vector<DescriptorSet>& descriptorSets() { return m_descriptorSets; }

private:
    Ref<const LogicalDevice> m_logicalDevice;
    DescriptorSetLayout m_layout;
    std::vector<DescriptorSet> m_descriptorSets;
};

} // namespace R3
