#if R3_VULKAN

#include "render/DescriptorSet.hpp"

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/DescriptorPool.hpp"
#include "render/DescriptorSetLayout.hpp"
#include "render/LogicalDevice.hpp"
#include "render/RenderSpecification.hpp"
#include "render/TextureBuffer.hpp"
#include "render/UniformBuffer.hpp"

namespace R3 {

std::vector<DescriptorSet> DescriptorSet::allocate(const DescriptorSetSpecification& spec) {
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

void DescriptorSet::free(const std::span<DescriptorSet> descriptorSets) {
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

void DescriptorSet::bindResources(const DescriptorSetBindingSpecification& spec) {
    // Write sets
    std::vector<vk::WriteDescriptorSet> descriptorWrites;
    descriptorWrites.reserve(spec.uniformDescriptors.size() + spec.textureDescriptors.size());

    // Buffer sets
    std::vector<vk::DescriptorBufferInfo> descriptorBufferInfos;
    descriptorBufferInfos.reserve(spec.uniformDescriptors.size());

    for (const auto& it : spec.uniformDescriptors) {
        auto& info = descriptorBufferInfos.emplace_back(vk::DescriptorBufferInfo{
            .buffer = it.uniform.as<vk::Buffer>(),
            .offset = it.offset,
            .range = it.range ?  it.range : vk::WholeSize,
        });

        descriptorWrites.emplace_back(vk::WriteDescriptorSet{
            .sType = vk::StructureType::eWriteDescriptorSet,
            .pNext = nullptr,
            .dstSet = as<vk::DescriptorSet>(),
            .dstBinding = it.binding,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = vk::DescriptorType::eUniformBuffer,
            .pImageInfo = nullptr,
            .pBufferInfo = &info,
            .pTexelBufferView = nullptr,
        });
    }

    // Image sets
    std::vector<vk::DescriptorImageInfo> descriptorImageInfos;
    descriptorImageInfos.reserve(spec.textureDescriptors.size());

    for (const auto& it : spec.textureDescriptors) {
        auto& info = descriptorImageInfos.emplace_back(vk::DescriptorImageInfo{
            .sampler = it.texture.sampler().as<vk::Sampler>(),
            .imageView = it.texture.textureView().as<vk::ImageView>(),
            .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
        });

        descriptorWrites.emplace_back(vk::WriteDescriptorSet{
            .sType = vk::StructureType::eWriteDescriptorSet,
            .pNext = nullptr,
            .dstSet = as<vk::DescriptorSet>(),
            .dstBinding = it.binding,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = vk::DescriptorType::eCombinedImageSampler,
            .pImageInfo = &info,
            .pBufferInfo = nullptr,
            .pTexelBufferView = nullptr,
        });
    }

    m_spec.logicalDevice->as<vk::Device>().updateDescriptorSets(descriptorWrites, {});
}

} // namespace R3

#endif // R3_VULKAN