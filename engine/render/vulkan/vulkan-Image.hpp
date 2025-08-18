#pragma once

#include <vulkan/vulkan_core.h>
#include "Types.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3::vulkan {

class Image {
public:
    void allocate(RenderContext& ctx,
                  VkFormat format,
                  VkExtent3D extent,
                  uint32 mipLevels,
                  uint32 sampleCount,
                  VkImageTiling tiling,
                  VkImageUsageFlags usage,
                  VkMemoryPropertyFlags properties);

    void free() noexcept;

    void copy(VkCommandBuffer cmd, VkBuffer buffer, const VkBufferImageCopy& bufferToImage);

    void copy(VkCommandBuffer cmd, VkImage image, const VkImageCopy& imageToImage);

    void generateMipMaps();

    VkImage image() const noexcept { return m_image; }

    VkImageView imageView() const noexcept { return m_imageView; }

    const VkExtent3D& extent() const noexcept { return m_extent; }

private:
    void* map();

    void unmap() noexcept;

private:
    RenderContext* m_ctx         = nullptr;
    VkImage m_image              = VK_NULL_HANDLE;
    VkDeviceMemory m_imageMemory = VK_NULL_HANDLE;
    VkImageView m_imageView      = VK_NULL_HANDLE;
    VkExtent3D m_extent          = {};
    void* m_mappedMemory         = nullptr;
};

} // namespace R3::vulkan
