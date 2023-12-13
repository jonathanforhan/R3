#if R3_VULKAN

#include "render/Shader.hpp"

#include <vulkan/vulkan.h>
#include <fstream>
#include <vector>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "render/LogicalDevice.hpp"

namespace R3 {

Shader::Shader(const ShaderSpecification& spec)
    : m_spec(spec) {
    std::ifstream ifs(m_spec.path.data(), std::ios::ate | std::ios::binary);
    CHECK(ifs.is_open());

    std::vector<char> bytes(ifs.tellg());
    ifs.seekg(0);
    ifs.read(bytes.data(), bytes.size());
    ifs.close();

    const VkShaderModuleCreateInfo shaderModuleCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
        .codeSize = static_cast<uint32>(bytes.size()),
        .pCode = reinterpret_cast<const uint32*>(bytes.data()),
    };

    VkShaderModule tmp;
    VkResult result =
        vkCreateShaderModule(m_spec.logicalDevice->as<VkDevice>(), &shaderModuleCreateInfo, nullptr, &tmp);
    ENSURE(result == VK_SUCCESS);
    setHandle(tmp);
}

Shader::~Shader() {
    if (validHandle()) {
        vkDestroyShaderModule(m_spec.logicalDevice->as<VkDevice>(), as<VkShaderModule>(), nullptr);
    }
}

} // namespace R3

#endif // R3_VULKAN