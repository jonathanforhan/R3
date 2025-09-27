#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include "engine/render/Shader.hpp"

namespace R3 {

struct ShaderMetadata {
    VkDescriptorSetLayout descriptorLayouts = VK_NULL_HANDLE;
    std::vector<VkPushConstantRange> pushConstantRanges;
    std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
};

} // namespace R3