#pragma once

#include <memory>
#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "api/Class.hpp"
#include "api/Types.hpp"
#include "vulkan-Fwd.hpp"

namespace R3::vulkan {

class DescriptorSet {
public:
    R3_CTOR_DEFAULT(DescriptorSet);

private:
    /// Private constructor - use allocate() instead
    DescriptorSet(RenderContext& ctx, VkDescriptorSet descriptorSet, std::shared_ptr<VkDescriptorPool> pool);

public:
    /// Static factory method to allocate DescriptorSets from a pool
    [[nodiscard]] static std::vector<DescriptorSet> allocate(RenderContext& ctx,
                                                             VkDescriptorSetLayout layout,
                                                             std::span<const VkDescriptorPoolSize> poolSizes,
                                                             uint32 count);

    void write(std::span<const VkWriteDescriptorSet> writes) noexcept;

    VkDescriptorSet descriptorSet() const noexcept { return m_descriptorSet; }

private:
    VkDevice m_device               = VK_NULL_HANDLE;
    VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;
    std::shared_ptr<VkDescriptorPool> m_pool;
};

} // namespace R3::vulkan
