#pragma once

#include <vulkan/vulkan.h>
#include "engine/api/Api.hpp"
#include "engine/api/Class.hpp"
#include "engine/api/Types.hpp"
#include "vulkan-Fwd.hpp"
#include "vulkan-Handle.hpp"

namespace R3::vulkan {

class R3_API Image {
public:
    R3_CTOR_DEFAULT(Image);
    R3_COPY_DELETE(Image);
    R3_MOVE_DEFAULT(Image);

    Image(VkImageCreateInfo imageInfo, VkImageAspectFlags aspectFlags, VkMemoryPropertyFlags properties);

    ~Image() noexcept;

    VkImage image() const noexcept { return m_image; }

    VkImageView imageView() const noexcept { return m_imageView; }

    // layer count is useful for cube maps (6 faces)
    void generateMipMaps(CommandBuffer& cmd, VkExtent2D extent, uint32 mipLevels, uint32 layerCount = 1);

private:
    Handle<VkImage> m_image;
    Handle<VkDeviceMemory> m_imageMemory;
    Handle<VkImageView> m_imageView;
    VkImageAspectFlags m_aspectFlags = {};
};

} // namespace R3::vulkan
