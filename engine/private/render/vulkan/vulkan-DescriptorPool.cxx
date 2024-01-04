#if R3_VULKAN

#include "render/DescriptorPool.hpp"

#include <vulkan/vulkan.hpp>
#include "render/LogicalDevice.hpp"

namespace R3 {

DescriptorPool::DescriptorPool(const DescriptorPoolSpecification& spec)
    : m_logicalDevice(&spec.logicalDevice) {
    m_layout = DescriptorSetLayout({
        .logicalDevice = *m_logicalDevice,
        .layoutBindings = spec.layoutBindings,
    });

    std::vector<vk::DescriptorPoolSize> poolSizes;

    for (const auto& binding : spec.layoutBindings) {
        auto it = std::find_if(poolSizes.begin(), poolSizes.end(), [&](auto& poolSize) {
            return uint32(poolSize.type) == uint32(binding.type);
        });

        if (it == poolSizes.end()) {
            poolSizes.emplace_back(vk::DescriptorType(binding.type), binding.count * spec.descriptorSetCount);
        } else {
            it->descriptorCount += binding.count * spec.descriptorSetCount;
        }
    }

    const vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo = {
        .sType = vk::StructureType::eDescriptorPoolCreateInfo,
        .pNext = nullptr,
        .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        .maxSets = spec.descriptorSetCount,
        .poolSizeCount = static_cast<uint32>(poolSizes.size()),
        .pPoolSizes = poolSizes.data(),
    };

    setHandle(m_logicalDevice->as<vk::Device>().createDescriptorPool(descriptorPoolCreateInfo));

    m_descriptorSets = DescriptorSet::allocate({
        .logicalDevice = *m_logicalDevice,
        .descriptorPool = *this,
        .descriptorSetLayout = m_layout,
        .descriptorSetCount = spec.descriptorSetCount,
    });
}

DescriptorPool::~DescriptorPool() {
    if (validHandle()) {
        m_logicalDevice->as<vk::Device>().destroyDescriptorPool(as<vk::DescriptorPool>());
    }
}

} // namespace R3

#endif // R3_VULKAN