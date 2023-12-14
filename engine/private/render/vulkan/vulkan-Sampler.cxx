#if R3_VULKAN

#include "render/Sampler.hpp"

#include <vulkan/vulkan.hpp>
#include "api/Ensure.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"

namespace R3 {

Sampler::Sampler(const SamplerSpecification& spec)
    : m_spec(spec) {
    vk::PhysicalDeviceProperties props = m_spec.physicalDevice->as<vk::PhysicalDevice>().getProperties();

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
        .maxLod = 0.0f,
        .borderColor = vk::BorderColor::eIntOpaqueBlack,
        .unnormalizedCoordinates = VK_FALSE,
    };

    setHandle(m_spec.logicalDevice->as<vk::Device>().createSampler(samplerCreateInfo));
}

Sampler::~Sampler() {
    if (validHandle()) {
        m_spec.logicalDevice->as<vk::Device>().destroySampler(as<vk::Sampler>());
    }
}

} // namespace R3

#endif // R3_VULKAN