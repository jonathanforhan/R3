#if R3_VULKAN

#include "render/Sampler.hxx"

#include <vulkan/vulkan.hpp>
#include "render/LogicalDevice.hxx"
#include "render/PhysicalDevice.hxx"

namespace R3 {

Sampler::Sampler(const SamplerSpecification& spec)
    : m_logicalDevice(&spec.logicalDevice) {
    vk::PhysicalDeviceProperties props = spec.physicalDevice.as<vk::PhysicalDevice>().getProperties();

    const vk::SamplerCreateInfo samplerCreateInfo = {
        .sType = vk::StructureType::eSamplerCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .magFilter = vk::Filter::eLinear,
        .minFilter = vk::Filter::eLinear,
        .mipmapMode = vk::SamplerMipmapMode::eLinear,
        .addressModeU = vk::SamplerAddressMode::eRepeat,
        .addressModeV = vk::SamplerAddressMode::eRepeat,
        .addressModeW = vk::SamplerAddressMode::eRepeat,
        .mipLodBias = 0.0f,
        .anisotropyEnable = VK_TRUE,
        .maxAnisotropy = props.limits.maxSamplerAnisotropy,
        .compareEnable = VK_FALSE,
        .compareOp = vk::CompareOp::eAlways,
        .minLod = 0.0f,
        .maxLod = static_cast<float>(spec.mipLevels),
        .borderColor = vk::BorderColor::eIntOpaqueBlack,
        .unnormalizedCoordinates = VK_FALSE,
    };

    setHandle(m_logicalDevice->as<vk::Device>().createSampler(samplerCreateInfo));
}

Sampler::~Sampler() {
    if (validHandle()) {
        m_logicalDevice->as<vk::Device>().destroySampler(as<vk::Sampler>());
    }
}

} // namespace R3

#endif // R3_VULKAN