// #if R3_VULKAN
//
// #include "api/Assert.hpp"
// #include "api/Types.hpp"
// #include "vulkan-Image.hxx"
// #include "vulkan-LogicalDevice.hxx"
// #include "vulkan-PhysicalDevice.hxx"
// #include "vulkan-Swapchain.hxx"
// #include "vulkan-VulkanObject.hxx"
// #include <type_traits>
// #include <vector>
// #include <vulkan/vulkan.h>
//
// namespace R3::vulkan {
//
// Image::Image(Image&& other) noexcept
//     : m_device(other.m_device),
//       m_imageView(other.m_imageView) {
//     other.m_device    = VK_NULL_HANDLE;
//     other.m_imageView = VK_NULL_HANDLE;
// }
//
// Image& Image::operator=(Image&& other) noexcept {
//     m_device          = other.m_device;
//     m_imageView       = other.m_imageView;
//     other.m_device    = VK_NULL_HANDLE;
//     other.m_imageView = VK_NULL_HANDLE;
// }
//
// Image::~Image() {
//     vkDestroyImageView(m_device, m_imageView, nullptr);
//     vkDestroyImage(m_device, m_handle, nullptr);
// }
//
// std::vector<Image> Image::acquireSwapchainImages(const ImageSpecification& spec) {
//     uint32 imageCount;
//     vkGetSwapchainImagesKHR(spec.device, spec.swapchain, &imageCount, nullptr);
//
//     std::vector<VkImage> images(imageCount);
//     vkGetSwapchainImagesKHR(spec.device, spec.swapchain, &imageCount, images.data());
//
//     // recreate image views and framebuffers with new images
//     std::vector<VkImageView> imageViews(images.size());
//
//     for (uint32 i = 0; i < images.size(); i++) {
//         VkImageViewCreateInfo imageViewCreateInfo = {
//             .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
//             .pNext    = nullptr,
//             .flags    = {},
//             .image    = images[i],
//             .viewType = VK_IMAGE_VIEW_TYPE_2D,
//             .format   = spec.format,
//             .components =
//                 {
//                     .r = VK_COMPONENT_SWIZZLE_IDENTITY,
//                     .g = VK_COMPONENT_SWIZZLE_IDENTITY,
//                     .b = VK_COMPONENT_SWIZZLE_IDENTITY,
//                     .a = VK_COMPONENT_SWIZZLE_IDENTITY,
//                 },
//             .subresourceRange =
//                 {
//                     .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
//                     .baseMipLevel   = 0,
//                     .levelCount     = 1,
//                     .baseArrayLayer = 0,
//                     .layerCount     = 1,
//                 },
//         };
//
//         VkResult result = vkCreateImageView(spec.device, &imageViewCreateInfo, nullptr, &imageViews[i]);
//         ENSURE(result == VK_SUCCESS);
//     }
//
//     std::vector<Image> finalImages(images.size());
//
//     for (uint32 i = 0; i < images.size(); i++) {
//         finalImages[i].m_handle = images[i];
//         finalImages[i].m_device = spec.device;
//         // finalImages[i].m_deviceMemory = VK_NULL_HANDLE;  /* implicit */
//         finalImages[i].m_imageView = imageViews[i];
//     }
// }
//
// Image&& Image::allocate(const ImageAllocateSpecification& spec) {
//     ASSERT(spec.mipLevels != 0);
//     ASSERT(spec.samples != 0);
//
//     const uint32 indices[] = {
//         spec.device.graphicsQueue().index(),
//         spec.device.presentationQueue().index(),
//     };
//
//     const VkImageCreateInfo imageCreateInfo = {
//         .sType     = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
//         .pNext     = nullptr,
//         .flags     = {},
//         .imageType = VK_IMAGE_TYPE_2D,
//         .format    = spec.format,
//         .extent =
//             {
//                 .width  = spec.width,
//                 .height = spec.height,
//                 .depth  = 1,
//             },
//         .mipLevels             = spec.mipLevels,
//         .arrayLayers           = 1,
//         .samples               = spec.samples,
//         .tiling                = VK_IMAGE_TILING_OPTIMAL,
//         .usage                 = spec.imageFlags,
//         .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
//         .queueFamilyIndexCount = static_cast<uint32>(std::size(indices)),
//         .pQueueFamilyIndices   = indices,
//         .initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED,
//     };
//
//     VkImage img;
//     VkResult result = vkCreateImage(spec.device, &imageCreateInfo, nullptr, &img);
//     ENSURE(result == VK_SUCCESS);
//
//     VkMemoryRequirements memoryRequirements;
//     vkGetImageMemoryRequirements(spec.device, img, &memoryRequirements);
//
//     const VkMemoryAllocateInfo memoryAllocateInfo = {
//         .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
//         .pNext           = nullptr,
//         .allocationSize  = memoryRequirements.size,
//         .memoryTypeIndex = spec.physicalDevice.queryMemoryType(memoryRequirements.memoryTypeBits, spec.memoryFlags),
//     };
//
//     VkDeviceMemory memory;
//     vkAllocateMemory(spec.device, &memoryAllocateInfo, nullptr, &memory);
//
//     vkBindImageMemory(spec.device, img, memory, 0);
//
//     VkImageViewCreateInfo imageViewCreateInfo = {
//         .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
//         .pNext    = nullptr,
//         .flags    = {},
//         .image    = img,
//         .viewType = VK_IMAGE_VIEW_TYPE_2D,
//         .format   = spec.format,
//         .components =
//             {
//                 .r = VK_COMPONENT_SWIZZLE_IDENTITY,
//                 .g = VK_COMPONENT_SWIZZLE_IDENTITY,
//                 .b = VK_COMPONENT_SWIZZLE_IDENTITY,
//                 .a = VK_COMPONENT_SWIZZLE_IDENTITY,
//             },
//         .subresourceRange =
//             {
//                 .aspectMask     = spec.aspectFlags,
//                 .baseMipLevel   = 0,
//                 .levelCount     = spec.mipLevels,
//                 .baseArrayLayer = 0,
//                 .layerCount     = 1,
//             },
//     };
//
//     VkImageView imageView;
//     VkResult result = vkCreateImageView(spec.device, &imageViewCreateInfo, nullptr, &imageView);
//     ENSURE(result == VK_SUCCESS);
//
//     Image image;
//     image.m_handle    = img;
//     image.m_device    = spec.device;
//     image.m_memory    = memory;
//     image.m_imageView = imageView;
//
//     return std::move(image);
// }
//
// } // namespace R3::vulkan
//
// #endif // R3_VULKAN
