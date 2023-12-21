#if R3_VULKAN

#include "render/Shader.hpp"

#include <fstream>
#include <vector>
#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/LogicalDevice.hpp"

namespace R3 {

Shader::Shader(const ShaderSpecification& spec)
    : m_logicalDevice(spec.logicalDevice) {
    std::ifstream ifs(spec.path.data(), std::ios::ate | std::ios::binary);
    CHECK(ifs.is_open());

    std::vector<char> bytes(ifs.tellg());
    ifs.seekg(0);
    ifs.read(bytes.data(), bytes.size());
    ifs.close();

    const vk::ShaderModuleCreateInfo shaderModuleCreateInfo = {
        .sType = vk::StructureType::eShaderModuleCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .codeSize = static_cast<uint32>(bytes.size()),
        .pCode = reinterpret_cast<const uint32*>(bytes.data()),
    };

    setHandle(m_logicalDevice->as<vk::Device>().createShaderModule(shaderModuleCreateInfo));
}

Shader::~Shader() {
    if (validHandle()) {
        m_logicalDevice->as<vk::Device>().destroyShaderModule(as<vk::ShaderModule>());
    }
}

} // namespace R3

#endif // R3_VULKAN