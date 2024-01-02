#if R3_VULKAN

#include "render/DescriptorSet.hxx"

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "core/Scene.hpp"
#include "render/DescriptorPool.hxx"
#include "render/DescriptorSetLayout.hxx"
#include "render/LogicalDevice.hxx"
#include "render/RenderSpecification.hxx"
#include "render/ResourceManager.hxx"

namespace R3 {

std::vector<DescriptorSet> DescriptorSet::allocate(const DescriptorSetSpecification& spec) {
    const std::vector<vk::DescriptorSetLayout> descriptorSetLayouts(
        spec.descriptorSetCount, spec.descriptorSetLayout.as<vk::DescriptorSetLayout>());

    const vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo = {
        .sType = vk::StructureType::eDescriptorSetAllocateInfo,
        .pNext = nullptr,
        .descriptorPool = spec.descriptorPool.as<vk::DescriptorPool>(),
        .descriptorSetCount = spec.descriptorSetCount,
        .pSetLayouts = descriptorSetLayouts.data(),
    };

    const auto allocatedDescriptorSets =
        spec.logicalDevice.as<vk::Device>().allocateDescriptorSets(descriptorSetAllocateInfo);
    CHECK(allocatedDescriptorSets.size() == spec.descriptorSetCount);

    std::vector<DescriptorSet> descriptorSets(spec.descriptorSetCount);

    for (usize i = 0; i < descriptorSets.size(); i++) {
        descriptorSets[i].m_logicalDevice = &spec.logicalDevice;
        descriptorSets[i].setHandle(allocatedDescriptorSets[i]);
    }

    return descriptorSets;
}

void DescriptorSet::bindResources(const DescriptorSetBindingSpecification& spec) {
    // Write sets
    std::vector<vk::WriteDescriptorSet> descriptorWrites;
    descriptorWrites.reserve(spec.uniformDescriptors.size() + spec.textureDescriptors.size());

    // Buffer sets
    std::vector<vk::DescriptorBufferInfo> descriptorBufferInfos;
    descriptorBufferInfos.reserve(spec.uniformDescriptors.size() + spec.storageDescriptors.size());

    for (const auto& it : spec.uniformDescriptors) {
        auto& uniform = ((ResourceManager*)CurrentScene->resourceManager)->getUniformById(it.uniform);

        auto& info = descriptorBufferInfos.emplace_back(vk::DescriptorBufferInfo{
            .buffer = uniform.as<vk::Buffer>(),
            .offset = it.offset,
            .range = it.range ? it.range : vk::WholeSize,
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

    for (const auto& it : spec.storageDescriptors) {
        auto& storage = ((ResourceManager*)CurrentScene->resourceManager)->getStorageBufferById(it.storage);

        auto& info = descriptorBufferInfos.emplace_back(vk::DescriptorBufferInfo{
            .buffer = storage.as<vk::Buffer>(),
            .offset = it.offset,
            .range = it.range ? it.range : vk::WholeSize,
        });

        descriptorWrites.emplace_back(vk::WriteDescriptorSet{
            .sType = vk::StructureType::eWriteDescriptorSet,
            .pNext = nullptr,
            .dstSet = as<vk::DescriptorSet>(),
            .dstBinding = it.binding,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = vk::DescriptorType::eStorageBuffer,
            .pImageInfo = nullptr,
            .pBufferInfo = &info,
            .pTexelBufferView = nullptr,
        });
    }

    // Image sets
    std::vector<vk::DescriptorImageInfo> descriptorImageInfos;
    descriptorImageInfos.reserve(spec.textureDescriptors.size());

    for (const auto& it : spec.textureDescriptors) {
        auto& texture = ((ResourceManager*)CurrentScene->resourceManager)->getTextureById(it.texture);

        auto& info = descriptorImageInfos.emplace_back(vk::DescriptorImageInfo{
            .sampler = texture.sampler().as<vk::Sampler>(),
            .imageView = texture.textureView().as<vk::ImageView>(),
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

    m_logicalDevice->as<vk::Device>().updateDescriptorSets(descriptorWrites, {});
}

} // namespace R3

#endif // R3_VULKAN