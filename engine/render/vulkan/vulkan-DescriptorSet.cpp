#include "vulkan-DescriptorSet.hpp"

#include <memory>
#include <new>
#include <span>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "api/Types.hpp"
#include "core/Engine.hpp"
#include "vulkan-Check.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3::vulkan {

DescriptorSet::DescriptorSet(RenderContext& ctx, VkDescriptorSet descriptorSet, std::shared_ptr<VkDescriptorPool> pool)
    : m_device(ctx.device()),
      m_descriptorSet(descriptorSet),
      m_pool(std::move(pool)) {}

std::vector<DescriptorSet> R3::vulkan::DescriptorSet::allocate(RenderContext& ctx,
                                                               VkDescriptorSetLayout layout,
                                                               std::span<const VkDescriptorPoolSize> poolSizes,
                                                               uint32 count) {
    std::shared_ptr<VkDescriptorPool> pool(new VkDescriptorPool, [device = ctx.device()](auto* p) noexcept {
        if (device && p) {
            vkDestroyDescriptorPool(device, *p, nullptr);
        }
    });

    // Create shared descriptor pool
    const VkDescriptorPoolCreateInfo poolInfo = {
        .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
        .maxSets       = count,
        .poolSizeCount = static_cast<uint32>(poolSizes.size()),
        .pPoolSizes    = poolSizes.data(),
    };
    VK_CHECK(vkCreateDescriptorPool(ctx.device(), &poolInfo, nullptr, &*pool));

    std::vector<VkDescriptorSetLayout> layouts(count, layout);

    // Allocate descriptor sets from the pool
    VkDescriptorSetAllocateInfo allocInfo = {
        .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext              = nullptr,
        .descriptorPool     = *pool,
        .descriptorSetCount = static_cast<uint32>(layouts.size()),
        .pSetLayouts        = layouts.data(),
    };

    std::vector<VkDescriptorSet> vkDescriptorSets(count);
    VK_CHECK(vkAllocateDescriptorSets(ctx.device(), &allocInfo, vkDescriptorSets.data()));

    std::vector<DescriptorSet> descriptorSets;
    descriptorSets.resize(count);

    for (uint32 i = 0; i < count; ++i) {
        descriptorSets[i] = std::move(DescriptorSet(ctx, vkDescriptorSets[i], pool));
    }

    return descriptorSets;
}

void DescriptorSet::write(std::span<const VkWriteDescriptorSet> writes) noexcept {
    vkUpdateDescriptorSets(m_device, static_cast<uint32>(writes.size()), writes.data(), 0, nullptr);
}

} // namespace R3::vulkan
