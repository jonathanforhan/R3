#if R3_VULKAN

#include "vulkan-DescriptorAllocator.hpp"
#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <Exception.hpp>
#include <Types.hpp>
#include "vulkan-Check.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3::vulkan {

void DescriptorAllocator::create(RenderContext& ctx, std::span<const VkDescriptorPoolSize> poolSizes, uint32 maxSets) {
    m_device = ctx.device();

    const VkDescriptorPoolCreateInfo poolInfo = {
        .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = {},
        .maxSets       = maxSets,
        .poolSizeCount = static_cast<uint32>(poolSizes.size()),
        .pPoolSizes    = poolSizes.data(),
    };
    VK_CHECK(vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptorPool));
}

void DescriptorAllocator::destroy() noexcept {
    if (m_device != VK_NULL_HANDLE) {
        if (m_layout != VK_NULL_HANDLE) {
            vkDestroyDescriptorSetLayout(m_device, m_layout, nullptr);
            m_layout = VK_NULL_HANDLE;
        }

        if (m_descriptorPool != VK_NULL_HANDLE) {
            vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
            m_descriptorPool = VK_NULL_HANDLE;
        }
    }
    m_device = VK_NULL_HANDLE;
}

std::vector<VkDescriptorSet> DescriptorAllocator::allocate(const VkDescriptorSetLayoutBinding& binding, uint32 count) {
    if (m_layout != VK_NULL_HANDLE) {
        throw Exception{__FUNCTION__ " FIXME"};
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo = {
        .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext        = nullptr,
        .flags        = {},
        .bindingCount = 1,
        .pBindings    = &binding,
    };

    VK_CHECK(vkCreateDescriptorSetLayout(m_device, &layoutInfo, nullptr, &m_layout));

    std::vector<VkDescriptorSetLayout> layouts(count, m_layout);

    VkDescriptorSetAllocateInfo descriptorInfo = {
        .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext              = nullptr,
        .descriptorPool     = m_descriptorPool,
        .descriptorSetCount = static_cast<uint32>(layouts.size()),
        .pSetLayouts        = layouts.data(),
    };

    std::vector<VkDescriptorSet> descriptorSets(layouts.size());
    VK_CHECK(vkAllocateDescriptorSets(m_device, &descriptorInfo, descriptorSets.data()));
    return descriptorSets;
}

} // namespace R3::vulkan

#endif // R3_VULKAN