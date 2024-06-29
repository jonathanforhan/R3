#if R3_VULKAN

#include "vulkan-ShaderModule.hxx"

#include "vulkan-LogicalDevice.hxx"
#include <api/Assert.hpp>
#include <api/Types.hpp>
#include <fstream>
#include <vector>

namespace R3::vulkan {

ShaderModule::ShaderModule(const ShaderModuleSpecification& spec)
    : m_device(spec.device.vk()),
      m_stage(spec.stage) {
    std::ifstream ifs(spec.path, std::ios::ate | std::ios::binary);
    ASSERT(ifs.is_open() && ifs.good());

    std::vector<char> bytes(ifs.tellg());
    ASSERT(bytes.size() % 4 == 0);

    ifs.seekg(0);
    ifs.read(bytes.data(), bytes.size());
    ifs.close();

    const VkShaderModuleCreateInfo shaderModuleCreateInfo = {
        .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext    = nullptr,
        .flags    = {},
        .codeSize = static_cast<uint32>(bytes.size()),
        .pCode    = reinterpret_cast<const uint32*>(bytes.data()),
    };

    VkResult result = vkCreateShaderModule(m_device, &shaderModuleCreateInfo, nullptr, &m_handle);
    ENSURE(result == VK_SUCCESS);
}

ShaderModule::~ShaderModule() {
    vkDestroyShaderModule(m_device, m_handle, nullptr);
}

} // namespace R3::vulkan

#endif // R3_VULKAN
