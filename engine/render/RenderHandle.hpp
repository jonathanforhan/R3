#pragma once

#if R3_VULKAN
extern "C" {
typedef struct VkDeviceMemory_T* VkDeviceMemory;
typedef struct VkBuffer_T* VkBuffer;
typedef struct VkImage_T* VkImage;
typedef struct VkImageView_T* VkImageView;
}
#endif

namespace R3 {

using RenderHandle = void*;

using DeviceMemoryRenderHandle = VkDeviceMemory;
using BufferRenderHandle       = VkBuffer;
using ImageRenderHandle        = VkImage;
using ImageViewRenderHandle    = VkImageView;

} // namespace R3