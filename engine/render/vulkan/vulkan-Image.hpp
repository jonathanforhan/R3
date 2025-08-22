#pragma once

#include <vulkan/vulkan_core.h>
#include "Types.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3::vulkan {

class Image {
public:
    void create(RenderContext& ctx,
                VkFormat format,
                VkExtent3D extent,
                uint32 mipLevels,
                uint32 sampleCount,
                VkImageTiling tiling,
                VkImageUsageFlags usage,
                VkMemoryPropertyFlags properties);

    void destroy() noexcept;

    void copy(VkCommandBuffer cmd, VkQueue queue, VkBuffer buffer, const VkBufferImageCopy& bufferToImage);

    void copy(VkCommandBuffer cmd, VkQueue queue, VkImage image, const VkImageCopy& imageToImage);

    void transition(VkCommandBuffer cmd,
                    VkQueue queue,
                    VkPipelineStageFlags srcStage,
                    VkPipelineStageFlags dstStage,
                    const VkImageMemoryBarrier& memoryBarrier);

    void generateMipMaps(VkCommandBuffer cmd, VkQueue queue);

    VkImage image() const noexcept { return m_image; }

    VkImageView imageView() const noexcept { return m_imageView; }

    const VkExtent3D& extent() const noexcept { return m_extent; }

private:
    VkDevice m_device            = VK_NULL_HANDLE;
    VkImage m_image              = VK_NULL_HANDLE;
    VkDeviceMemory m_imageMemory = VK_NULL_HANDLE;
    VkImageView m_imageView      = VK_NULL_HANDLE;
    VkExtent3D m_extent          = {};
    uint32 m_mipLevels           = 0;
};

} // namespace R3::vulkan
