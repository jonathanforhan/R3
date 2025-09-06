#pragma once

#include <vulkan/vulkan.h>
#include "engine/render/Flags.hpp"

// Buffer
#define TO_VK_BUFFER_USAGE_FLAGS(usage) ((VkBufferUsageFlags)((usage) & 0x0000'FFFF))
#define TO_VK_MEMORY_FLAGS(usage)       (((VkMemoryPropertyFlags)((usage) >> 16)) & 0xFF)

// Image
#define TO_VK_IMAGE_USAGE_FLAGS(usage) ((VkImageUsageFlags)((usage) & 0x0000'FFFF) | VK_IMAGE_USAGE_SAMPLED_BIT)
#define TO_VK_IMAGE_TYPE(type)         ((type) == ImageType::ImageCube ? VK_IMAGE_TYPE_2D : (VkImageType)(type))
#define TO_VK_IMAGE_VIEW_TYPE(type)    ((VkImageViewType)(type))
#define TO_VK_IMAGE_ASPECT(usage)                                                                 \
    ((VkImageUsageFlags)((usage) & ImageUsage::DepthStencilAttachment ? VK_IMAGE_ASPECT_DEPTH_BIT \
                                                                      : VK_IMAGE_ASPECT_COLOR_BIT))
/* ^^^ TODO should support stencil and maybe multi-plane in the future */

#define TO_VK_FORMAT(format) ((VkFormat)(format))