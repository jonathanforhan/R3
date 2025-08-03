#pragma once

#include <cstdint>
#include <span>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace R3 {

class RenderContext;

enum class ShaderStage { Vertex, Fragment, Geometry, TessellationControl, TessellationEvaluation, Compute };

class Shader {
public:
    void createFromFile(RenderContext& ctx, const std::string& filename, ShaderStage type) noexcept(false);

    void createFromSource(RenderContext& ctx, std::span<const uint32_t> spirvCode, ShaderStage type) noexcept(false);

    void destroy() noexcept(true);

    VkShaderModule handle() const { return m_shaderModule; }
    ShaderStage type() const { return m_type; }
    VkShaderStageFlagBits stage() const;

    bool isValid() const { return m_shaderModule != VK_NULL_HANDLE; }

private:
    VkShaderStageFlagBits getStageFlags(ShaderStage type) const;
    std::vector<uint32_t> readFile(const std::string& filename) const;

private:
    VkDevice m_device             = VK_NULL_HANDLE;
    VkShaderModule m_shaderModule = VK_NULL_HANDLE;
    ShaderStage m_type            = ShaderStage::Vertex;
};

} // namespace R3