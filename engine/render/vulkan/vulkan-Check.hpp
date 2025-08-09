#pragma once

#if R3_VULKAN

#include <format>
#include <vulkan/vulkan_core.h>
#include "Exception.hpp"

#define VK_CHECK(_Exp)                                    \
    if (VkResult _result = (_Exp); _result != VK_SUCCESS) \
    throw ::R3::Exception(std::format(#_Exp " returned: ", static_cast<int>(_result)))

#endif // R3_VULKAN
