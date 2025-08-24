#include "vulkan-Shader.hpp"

#include <cstdint>
#include <filesystem>
#include <format>
#include <fstream>
#include <ios>
#include <istream>
#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "api/Exception.hpp"
#include "vulkan-Check.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3::vulkan {

Shader::Shader(RenderContext& ctx, const std::filesystem::path& filename, VkShaderStageFlags type) {
    auto spirvCode = readFile(filename);
    createFromSource(ctx, spirvCode, type);
}

Shader::~Shader() noexcept {
    if (m_device) {
        vkDestroyShaderModule(m_device, m_shaderModule, nullptr);
    }
}

void Shader::createFromSource(RenderContext& ctx, std::span<const uint32_t> spirvCode, VkShaderStageFlags type) {
    m_device = ctx.device();
    m_type   = type;

    VkShaderModuleCreateInfo shaderModuleCreateInfo = {
        .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext    = nullptr,
        .flags    = {},
        .codeSize = spirvCode.size() * sizeof(uint32_t),
        .pCode    = spirvCode.data(),
    };

    VK_CHECK(vkCreateShaderModule(m_device, &shaderModuleCreateInfo, nullptr, &*m_shaderModule));
}

std::vector<uint32_t> Shader::readFile(const std::filesystem::path& filename) const {
    std::ifstream file{filename, std::ios::ate | std::ios::binary};

    if (!(file.is_open() && file.good())) {
        throw Exception{std::format("Failed to open shader file: {}", filename.string())};
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

    file.seekg(0);
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
    file.close();

    return buffer;
}

} // namespace R3::vulkan
