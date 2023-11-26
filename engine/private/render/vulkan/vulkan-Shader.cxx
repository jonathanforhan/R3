#if R3_VULKAN

#include "render/Shader.hpp"

#include <vulkan/vulkan.h>
#include <fstream>
#include <vector>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
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

    VkShaderModuleCreateInfo shaderModuleCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0U,
        .codeSize = static_cast<uint32>(bytes.size()),
        .pCode = reinterpret_cast<const uint32*>(bytes.data()),
    };

    ENSURE(vkCreateShaderModule(m_spec.logicalDevice->handle<VkDevice>(),
                                &shaderModuleCreateInfo,
                                nullptr,
                                handlePtr<VkShaderModule*>()) == VK_SUCCESS);
}

void Shader::destroy() {
    vkDestroyShaderModule(m_spec.logicalDevice->handle<VkDevice>(), handle<VkShaderModule>(), nullptr);
}

} // namespace R3

#endif // R3_VULKAN