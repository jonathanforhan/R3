#if R3_VULKAN

#include "render/Shader.hpp"

#include <vulkan/vulkan.hpp>
#include <fstream>
#include "api/Check.hpp"
#include "render/LogicalDevice.hpp"

namespace R3 {

void Shader::create(const ShaderSpecification& spec) {
    CHECK(spec.logicalDevice != nullptr);
    m_spec = spec;

    std::ifstream ifs(m_spec.path.data(), std::ios::ate | std::ios::binary);
    CHECK(ifs.is_open());

    std::vector<char> bytes(ifs.tellg());
    ifs.seekg(0);
    ifs.read(bytes.data(), bytes.size());
    ifs.close();

    vk::ShaderModuleCreateInfo shaderModuleCreateInfo = {
        .sType = vk::StructureType::eShaderModuleCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .codeSize = static_cast<uint32>(bytes.size()),
        .pCode = reinterpret_cast<const uint32*>(bytes.data()),
    };

    setHandle(m_spec.logicalDevice->as<vk::Device>().createShaderModule(shaderModuleCreateInfo));
}

void Shader::destroy() {
    m_spec.logicalDevice->as<vk::Device>().destroyShaderModule(as<vk::ShaderModule>());
}

} // namespace R3

#endif // R3_VULKAN