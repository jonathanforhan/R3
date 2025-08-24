#pragma once

#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "api/Types.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3::vulkan {

class DescriptorAllocator {
public:
    void create(RenderContext& ctx, std::span<const VkDescriptorPoolSize> poolSizes, uint32 maxSets);

    void destroy() noexcept;

    std::vector<VkDescriptorSet> allocate(std::span<const VkDescriptorSetLayoutBinding> bindings, uint32 count);

    VkDescriptorSetLayout layout() const noexcept { return m_layout; }

private:
    VkDevice m_device                 = VK_NULL_HANDLE;
    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_layout    = VK_NULL_HANDLE;
};

} // namespace R3::vulkan