#if R3_VULKAN

#include "vulkan-ShaderModule.hxx"

#include "vulkan-LogicalDevice.hxx"
#include <api/Log.hpp>
#include <api/Result.hpp>
#include <api/Types.hpp>
#include <expected>
#include <fstream>
#include <vector>

namespace R3::vulkan {

Result<ShaderModule> ShaderModule::create(const ShaderModuleSpecification& spec) {
    ShaderModule self;
    self.m_device = spec.device.vk();
    self.m_stage  = spec.stage;

    std::ifstream ifs(spec.path, std::ios::ate | std::ios::binary);
    if (!(ifs.is_open() && ifs.good())) {
        R3_LOG(Error, "failure to open file {}", spec.path);
        return std::unexpected(Error::InvalidFilepath);
    }

    std::vector<char> bytes(ifs.tellg());

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

    if (VkResult result = vkCreateShaderModule(self.m_device, &shaderModuleCreateInfo, nullptr, &self.m_handle)) {
        R3_LOG(Error, "vkCreateShaderModule failure {}", (int)result);
        return std::unexpected(Error::InitializationFailure);
    }

    return self;
}

ShaderModule::~ShaderModule() {
    vkDestroyShaderModule(m_device, m_handle, nullptr);
}

} // namespace R3::vulkan

#endif // R3_VULKAN
