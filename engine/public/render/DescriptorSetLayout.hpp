#pragma once

#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct DescriptorSetLayoutSpecification {
    const LogicalDevice& logicalDevice;
};

class DescriptorSetLayout : public NativeRenderObject {
public:
    DescriptorSetLayout() = default;
    DescriptorSetLayout(const DescriptorSetLayoutSpecification& spec);
    DescriptorSetLayout(DescriptorSetLayout&&) noexcept = default;
    DescriptorSetLayout& operator=(DescriptorSetLayout&&) noexcept = default;
    ~DescriptorSetLayout();

private:
    Ref<const LogicalDevice> m_logicalDevice;
};

} // namespace R3