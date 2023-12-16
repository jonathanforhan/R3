#if R3_VULKAN

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/DescriptorSetLayout.hpp"
#include "render/LogicalDevice.hpp"

namespace R3 {

DescriptorSetLayout::DescriptorSetLayout(const DescriptorSetLayoutSpecification& spec)
    : m_spec(spec) {
    constexpr std::array<vk::DescriptorSetLayoutBinding, 6> descriptorSetLayoutBindings = {
        // UBO
        vk::DescriptorSetLayoutBinding{
            .binding = 0,
            .descriptorType = vk::DescriptorType::eUniformBuffer,
            .descriptorCount = 1,
            .stageFlags = vk::ShaderStageFlagBits::eVertex,
        },
        // Ambient Sampler
        vk::DescriptorSetLayoutBinding{
            .binding = 1,
            .descriptorType = vk::DescriptorType::eCombinedImageSampler,
            .descriptorCount = 1,
            .stageFlags = vk::ShaderStageFlagBits::eFragment,
        },
        // Normal Sampler
        vk::DescriptorSetLayoutBinding{
            .binding = 2,
            .descriptorType = vk::DescriptorType::eCombinedImageSampler,
            .descriptorCount = 1,
            .stageFlags = vk::ShaderStageFlagBits::eFragment,
        },
        // Metallic Roughness Sampler
        vk::DescriptorSetLayoutBinding{
            .binding = 3,
            .descriptorType = vk::DescriptorType::eCombinedImageSampler,
            .descriptorCount = 1,
            .stageFlags = vk::ShaderStageFlagBits::eFragment,
        },
        // Ambient Occlusion Sampler
        vk::DescriptorSetLayoutBinding{
            .binding = 4,
            .descriptorType = vk::DescriptorType::eCombinedImageSampler,
            .descriptorCount = 1,
            .stageFlags = vk::ShaderStageFlagBits::eFragment,
        },
        // Emission Sampler
        vk::DescriptorSetLayoutBinding{
            .binding = 5,
            .descriptorType = vk::DescriptorType::eCombinedImageSampler,
            .descriptorCount = 1,
            .stageFlags = vk::ShaderStageFlagBits::eFragment,
        },
    };

    const vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
        .sType = vk::StructureType::eDescriptorSetLayoutCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .bindingCount = static_cast<uint32>(descriptorSetLayoutBindings.size()),
        .pBindings = descriptorSetLayoutBindings.data(),
    };

    setHandle(m_spec.logicalDevice->as<vk::Device>().createDescriptorSetLayout(descriptorSetLayoutCreateInfo));
}

DescriptorSetLayout::~DescriptorSetLayout() {
    if (validHandle()) {
        m_spec.logicalDevice->as<vk::Device>().destroyDescriptorSetLayout(as<vk::DescriptorSetLayout>());
    }
}

} // namespace R3

#endif // R3_VULKAN
