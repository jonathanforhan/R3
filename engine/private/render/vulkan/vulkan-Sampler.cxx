#if R3_VULKAN

#include "render/Sampler.hpp"

#include <vulkan/vulkan.h>
#include "api/Ensure.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"

namespace R3 {

Sampler::Sampler(const SamplerSpecification& spec)
    : m_spec(spec) {
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(m_spec.physicalDevice->as<VkPhysicalDevice>(), &props);

    const VkSamplerCreateInfo samplerCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .mipLodBias = 0.0f,
        .anisotropyEnable = VK_TRUE,
        .maxAnisotropy = props.limits.maxSamplerAnisotropy,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_ALWAYS,
        .minLod = 0.0f,
        .maxLod = 0.0f,
        .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE,
    };

    VkSampler tmp;
    VkResult result = vkCreateSampler(m_spec.logicalDevice->as<VkDevice>(), &samplerCreateInfo, nullptr, &tmp);
    ENSURE(result == VK_SUCCESS);
    setHandle(tmp);
}

Sampler::~Sampler() {
    if (validHandle()) {
        vkDestroySampler(m_spec.logicalDevice->as<VkDevice>(), as<VkSampler>(), nullptr);
    }
}

} // namespace R3

#endif // R3_VULKAN