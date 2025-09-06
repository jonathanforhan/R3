#pragma once

namespace R3 {
using RenderHandle = void*;
} // namespace R3

#if R3_VULKAN
extern "C" typedef struct VkDeviceMemory_T* VkDeviceMemory;
extern "C" typedef struct VkBuffer_T* VkBuffer;
extern "C" typedef struct VkImage_T* VkImage;
extern "C" typedef struct VkImageView_T* VkImageView;

namespace R3 {
using DeviceMemoryRenderHandle = VkDeviceMemory;
using BufferRenderHandle       = VkBuffer;
using ImageRenderHandle        = VkImage;
using ImageViewRenderHandle    = VkImageView;
} // namespace R3
#endif