#if R3_VULKAN
#pragma once

#include <vulkan/vulkan.hpp>

namespace R3::vulkan {

vk::Format getSupportedDepthFormat(vk::PhysicalDevice gpu, vk::ImageTiling tiling, vk::FormatFeatureFlags features);

} // namespace R3::vulkan

#endif // R3_VULKAN