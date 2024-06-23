#pragma once

#if R3_VULKAN

#include "vulkan-fwd.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Construct.hpp>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

struct ShaderModuleSpecification {
    const LogicalDevice& device;
    const char* path;
    VkShaderStageFlagBits stage;
};

class ShaderModule : public VulkanObject<VkShaderModule> {
public:
    DEFAULT_CONSTRUCT(ShaderModule);
    NO_COPY(ShaderModule);
    DEFAULT_MOVE(ShaderModule);

    ShaderModule(const ShaderModuleSpecification& spec);

    ~ShaderModule();

    constexpr VkShaderStageFlagBits stage() const { return m_stage; }

private:
    VkDevice m_device             = VK_NULL_HANDLE;
    VkShaderStageFlagBits m_stage = {};
};

} // namespace R3::vulkan

#endif // R3_VULKAN
