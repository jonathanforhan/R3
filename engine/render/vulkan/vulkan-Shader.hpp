#pragma once

#include <span>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "Types.hpp"

namespace R3::vulkan {

class RenderContext;

class Shader {
public:
    void createFromFile(RenderContext& ctx, const std::string& filename, VkShaderStageFlags type);

    void createFromSource(RenderContext& ctx, std::span<const uint32> spirvCode, VkShaderStageFlags type);

    void destroy() noexcept;

    VkShaderModule shader() const noexcept { return m_shaderModule; }

    VkShaderStageFlags type() const noexcept { return m_type; }

private:
    std::vector<uint32> readFile(const std::string& filename) const;

private:
    VkDevice m_device             = VK_NULL_HANDLE;
    VkShaderModule m_shaderModule = VK_NULL_HANDLE;
    VkShaderStageFlags m_type;
};

} // namespace R3::vulkan