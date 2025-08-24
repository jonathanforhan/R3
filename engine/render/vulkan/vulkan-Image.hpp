#pragma once

#include <vulkan/vulkan_core.h>
#include "api/Class.hpp"
#include "api/Types.hpp"
#include "vulkan-CommandBuffer.hpp"
#include "vulkan-Handle.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3::vulkan {

class Image {
public:
    R3_CTOR_DEFAULT(Image);
    R3_COPY_DELETE(Image);
    R3_MOVE_DEFAULT(Image);

    Image(RenderContext& ctx,
          VkFormat format,
          VkExtent2D extent,
          uint32 mipLevels,
          VkSampleCountFlagBits sampleCount,
          VkImageTiling tiling,
          VkImageUsageFlags usage,
          VkImageAspectFlags aspectFlags,
          VkMemoryPropertyFlags properties);

    ~Image() noexcept;

    void generateMipMaps(CommandBuffer& cmd, VkQueue queue);

    VkImage image() const noexcept { return m_image; }

    VkImageView imageView() const noexcept { return m_imageView; }

    const VkExtent2D& extent() const noexcept { return m_extent; }

private:
    Handle<VkDevice> m_device;
    Handle<VkImage> m_image;
    Handle<VkDeviceMemory> m_imageMemory;
    Handle<VkImageView> m_imageView;
    VkExtent2D m_extent = {};
    uint32 m_mipLevels  = 0;
};

} // namespace R3::vulkan
