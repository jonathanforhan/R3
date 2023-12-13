#if R3_VULKAN

#include <vulkan/vulkan.h>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "render/DescriptorSetLayout.hpp"
#include "render/LogicalDevice.hpp"

namespace R3 {

DescriptorSetLayout::DescriptorSetLayout(const DescriptorSetLayoutSpecification& spec)
    : m_spec(spec) {
    const VkDescriptorSetLayoutBinding uboDescriptorSetLayoutBinding = {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .pImmutableSamplers = nullptr,
    };

    const VkDescriptorSetLayoutBinding samplerDescriptorSetLayoutBinding = {
        .binding = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        .pImmutableSamplers = nullptr,
    };

    const VkDescriptorSetLayoutBinding bindings[2]{
        uboDescriptorSetLayoutBinding,
        samplerDescriptorSetLayoutBinding,
    };

    const VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
        .bindingCount = 2,
        .pBindings = bindings,
    };

    VkDescriptorSetLayout tmp;
    VkResult result = vkCreateDescriptorSetLayout(
        m_spec.logicalDevice->as<VkDevice>(), &descriptorSetLayoutCreateInfo, nullptr, &tmp);
    ENSURE(result == VK_SUCCESS);

    setHandle(tmp);
}

DescriptorSetLayout::~DescriptorSetLayout() {
    if (validHandle()) {
        vkDestroyDescriptorSetLayout(m_spec.logicalDevice->as<VkDevice>(), as<VkDescriptorSetLayout>(), nullptr);
    }
}

} // namespace R3

#endif // R3_VULKAN
