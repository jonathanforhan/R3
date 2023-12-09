#if R3_VULKAN

#include "render/DescriptorSet.hpp"

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/DescriptorPool.hpp"
#include "render/DescriptorSetLayout.hpp"
#include "render/LogicalDevice.hpp"
#include "render/RenderSpecification.hpp"
#include "render/UniformBuffer.hpp"

namespace R3 {

std::vector<DescriptorSet> DescriptorSet::allocate(const DescriptorSetSpecification& spec) {
    CHECK(spec.logicalDevice != nullptr);
    CHECK(spec.descriptorPool != nullptr);
    CHECK(spec.descriptorSetLayout != nullptr);

    std::vector<vk::DescriptorSetLayout> descriptorSetLayouts(spec.descriptorSetCount,
                                                              spec.descriptorSetLayout->as<vk::DescriptorSetLayout>());

    vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo = {
        .sType = vk::StructureType::eDescriptorSetAllocateInfo,
        .pNext = nullptr,
        .descriptorPool = spec.descriptorPool->as<vk::DescriptorPool>(),
        .descriptorSetCount = spec.descriptorSetCount,
        .pSetLayouts = descriptorSetLayouts.data(),
    };

    auto allocatedDescriptorSets =
        spec.logicalDevice->as<vk::Device>().allocateDescriptorSets(descriptorSetAllocateInfo);
    CHECK(allocatedDescriptorSets.size() == spec.descriptorSetCount);

    std::vector<DescriptorSet> descriptorSets(spec.descriptorSetCount);

    for (usize i = 0; i < descriptorSets.size(); i++) {
        descriptorSets[i].m_spec = spec;
        descriptorSets[i].setHandle(allocatedDescriptorSets[i]);
    }

    return descriptorSets;
}

void DescriptorSet::free(std::vector<DescriptorSet>& descriptorSets) {
    std::vector<vk::DescriptorSet> sets;
    for (const auto& descriptorSet : descriptorSets) {
        sets.push_back(descriptorSet.as<vk::DescriptorSet>());
    }
    auto& spec = descriptorSets.front().m_spec;
    spec.logicalDevice->as<vk::Device>().freeDescriptorSets(spec.descriptorPool->as<vk::DescriptorPool>(), sets);
}

void DescriptorSet::free() {
    m_spec.logicalDevice->as<vk::Device>().freeDescriptorSets(m_spec.descriptorPool->as<vk::DescriptorPool>(),
                                                              {as<vk::DescriptorSet>()});
}

void DescriptorSet::bindUniform(const UniformBuffer& uniform, uint32 binding, usize offset, usize range) {
    vk::DescriptorBufferInfo descriptorBufferInfo = {
        .buffer = uniform.as<vk::Buffer>(),
        .offset = offset,
        .range = range ? range : vk::WholeSize,
    };

    vk::WriteDescriptorSet descriptorWrite = {
        .sType = vk::StructureType::eWriteDescriptorSet,
        .pNext = nullptr,
        .dstSet = as<vk::DescriptorSet>(),
        .dstBinding = binding,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = vk::DescriptorType::eUniformBuffer,
        .pImageInfo = nullptr,
        .pBufferInfo = &descriptorBufferInfo,
        .pTexelBufferView = nullptr,
    };

    m_spec.logicalDevice->as<vk::Device>().updateDescriptorSets(descriptorWrite, {});
}

} // namespace R3

#endif // R3_VULKAN