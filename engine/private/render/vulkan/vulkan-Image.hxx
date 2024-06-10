// #pragma once
//
// #if R3_VULKAN
//
// #include "vulkan-Buffer.hxx"
// #include "vulkan-fwd.hxx"
// #include "vulkan-VulkanObject.hxx"
// #include <api/Construct.hpp>
// #include <api/Types.hpp>
// #include <vector>
// #include <vulkan/vulkan.h>
//
// namespace R3::vulkan {
//
// struct ImageSpecification {
//     LogicalDevice& device;
//     const Swapchain& swapchain;
//     VkFormat format;
// };
//
// struct ImageAllocateSpecification {
//     const PhysicalDevice& physicalDevice; /**< PhysicalDevice */
//     const LogicalDevice& device;          /**< LogicalDevice */
//     usize size;                           /**< Image size in bytes of Image */
//     VkFormat format;                      /**< Image Format */
//     uint32 width;                         /**< Image width */
//     uint32 height;                        /**< Image height */
//     uint32 mipLevels = 1;                 /**< Image Mipmap Levels */
//     VkSampleCountFlagBits samples;        /**< Image Samples (1, 2, 4, ... 64) */
//     VkImageUsageFlagBits imageFlags;      /**< Usage flags */
//     VkMemoryPropertyFlagBits memoryFlags; /**< Memory flags */
//     VkImageAspectFlagBits aspectFlags;    /**< Aspect flags */
// };
//
// class Image : public VulkanObject<VkImage> {
// public:
//     DEFAULT_CONSTRUCT(Image);
//     NO_COPY(Image);
//     DEFAULT_MOVE(Image);
//
//     Image(Image&& other) noexcept;
//
//     Image& operator=(Image&& other) noexcept;
//
//     ~Image();
//
//     static std::vector<Image> acquireSwapchainImages(const ImageSpecification& spec);
//
//     static Image&& allocate(const ImageAllocateSpecification& spec);
//
//     // static void copy(const ImageCopySpecification& spec);
//
//     // static void transition(const ImageLayoutTransitionSpecification& spec);
//
//     // static void generateMipMaps(const ImageMipmapSpecification& spec);
// private:
//     VkDevice m_device       = VK_NULL_HANDLE;
//     VkImageView m_imageView = VK_NULL_HANDLE;
// };
//
// } // namespace R3::vulkan
//
// #endif // R3_VULKAN
