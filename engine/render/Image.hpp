#pragma once

#include <span>
#include <vulkan/vulkan_core.h>
#include "Flags.hpp"
#include "Types.hpp"

namespace R3 {

class RenderContext;

class Image {
public:
    void allocate(RenderContext& ctx,
                  VkFormat format,
                  uvec2 extent,
                  uint32 mipLevels,
                  uint32 sampleCount,
                  ImageTiling tiling,
                  ImageUsage usage,
                  MemoryProperties properties);

    void free() noexcept;

    void copy(const void* src, usize size);

    void generateMipMaps();

    VkImage image() noexcept { return m_image; }

    VkImageView imageView() noexcept { return m_imageView; }

    uvec2 extent() const noexcept { return m_extent; }

    bool isValid() const noexcept { return m_image != nullptr; }

private:
    void* map();

    void unmap() noexcept;

private:
    VkDevice m_device                 = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkImage m_image                   = VK_NULL_HANDLE;
    VkImageView m_imageView           = VK_NULL_HANDLE;
    VkDeviceMemory m_imageMemory      = VK_NULL_HANDLE;
    uvec2 m_extent                    = {0, 0};
    void* m_mappedMemory              = nullptr;
};

} // namespace R3
