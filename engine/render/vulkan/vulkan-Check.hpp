#pragma once

#include <format>
#include <vulkan/vulkan.h>
#include "Exception.hpp"

#define VK_CHECK(_Exp)                                    \
    if (VkResult _result = (_Exp); _result != VK_SUCCESS) \
    throw Exception(std::format(#_Exp " returned: ", static_cast<int>(_result)))
