#pragma once

#include <cstdint>
#include <span>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "Flags.hpp"
#include "Types.hpp"

namespace R3 {

class RenderContext;

class Shader {
public:
    void createFromFile(RenderContext& ctx, const std::string& filename, ShaderStage type);

    void createFromSource(RenderContext& ctx, std::span<const uint32> spirvCode, ShaderStage type);

    void destroy() noexcept;

    VkShaderModule handle() noexcept { return m_shaderModule; }

    ShaderStage type() const noexcept { return m_type; }

    bool isValid() const noexcept { return m_shaderModule != VK_NULL_HANDLE; }

private:
    std::vector<uint32> readFile(const std::string& filename) const;

private:
    VkDevice m_device             = VK_NULL_HANDLE;
    VkShaderModule m_shaderModule = VK_NULL_HANDLE;
    ShaderStage m_type            = ShaderStageFlags::Vertex;
};

} // namespace R3