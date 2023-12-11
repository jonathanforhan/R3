#if R3_VULKAN

#include "render/Sampler.hpp"

#include <vulkan/vulkan.hpp>
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"

namespace R3 {

Sampler::Sampler(const SamplerSpecification& spec)
    : m_spec(spec) {
    vk::SamplerCreateInfo samplerCreateInfo = {
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
        .anisotropyEnable = vk::True,
        .maxAnisotropy = m_spec.physicalDevice->as<vk::PhysicalDevice>().getProperties().limits.maxSamplerAnisotropy,
        .compareEnable = vk::False,
        .compareOp = vk::CompareOp::eAlways,
        .minLod = 0.0f,
        .maxLod = 0.0f,
        .borderColor = vk::BorderColor::eIntOpaqueBlack,
        .unnormalizedCoordinates = vk::False,
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