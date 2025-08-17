#if R3_VULKAN

#include "render/Shader.hpp"

#include <cstdint>
#include <format>
#include <fstream>
#include <ios>
#include <istream>
#include <span>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "Exception.hpp"
#include "render/Flags.hpp"
#include "render/RenderContext.hpp"
#include "vulkan-Check.hpp"

namespace R3 {

void Shader::createFromFile(RenderContext& ctx, const std::string& filename, ShaderStage type) {
    auto spirvCode = readFile(filename);
    createFromSource(ctx, spirvCode, type);
}

void Shader::createFromSource(RenderContext& ctx, std::span<const uint32_t> spirvCode, ShaderStage type) {
    m_device = ctx.device();
    m_type   = type;

    VkShaderModuleCreateInfo shaderModuleCreateInfo = {
        .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext    = nullptr,
        .flags    = {},
        .codeSize = spirvCode.size() * sizeof(uint32_t),
        .pCode    = spirvCode.data(),
    };

    VK_CHECK(vkCreateShaderModule(m_device, &shaderModuleCreateInfo, nullptr, &m_shaderModule));
}

void Shader::destroy() noexcept {
    if (m_shaderModule != VK_NULL_HANDLE && m_device != VK_NULL_HANDLE) {
        vkDestroyShaderModule(m_device, m_shaderModule, nullptr);
        m_shaderModule = VK_NULL_HANDLE;
    }

    m_device = VK_NULL_HANDLE;
}

std::vector<uint32_t> Shader::readFile(const std::string& filename) const {
    std::ifstream file{filename, std::ios::ate | std::ios::binary};

    if (!(file.is_open() && file.good())) {
        throw Exception{std::format("Failed to open shader file: {}", filename)};
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

    file.seekg(0);
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
    file.close();

    return buffer;
}

} // namespace R3

#endif // R3_VULKAN