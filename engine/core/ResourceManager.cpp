#include "ResourceManager.hpp"

#include <cstdint>
#include <map>
#include <utility>
#include <vector>
#include <vulkan/vulkan.h>
#include <entt/resource/cache.hpp>
#include "api/Assert.hpp"
#include "api/Hash.hpp"
#include "api/Types.hpp"
#include "core/Engine.hpp"
#include "render/Texture.hpp"
#include "render/vulkan/vulkan-DescriptorSet.hpp"
#include "render/vulkan/vulkan-RenderContext.hpp"

namespace R3 {

uint32 ResourceManager::bindTexture(hash::uuid id, const Texture& texture) {
    if (m_textureBindMap.contains(id)) {
        uint32 index = m_textureBindMap[id];
        m_textureBindSlots[index].first++;
        return index;
    }

    vulkan::RenderContext& ctx = GEngine()->RenderContext<vulkan::RenderContext>();

    uint32 slot;
    if (m_textureFreeBindSlots.empty()) {
        slot = static_cast<uint32>(m_textureBindSlots.size());
        m_textureBindSlots.emplace_back(1, id);
    } else {
        slot = m_textureFreeBindSlots.back();
        m_textureFreeBindSlots.pop_back();
        m_textureBindSlots[slot] = std::pair{1, id};
    }
    m_textureBindMap.emplace(id, slot);

    R3_ASSERT(slot < ctx.maxTextureSamplerBindings(), "Texture bind slot out of range");

    const VkDescriptorImageInfo imageInfo = {
        .sampler     = texture.samplerHandle(),
        .imageView   = texture.imageViewHandle(),
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };
    VkWriteDescriptorSet descriptorWrite = {
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext            = nullptr,
        .dstSet           = nullptr /* Descriptor set to update */,
        .dstBinding       = 2,
        .dstArrayElement  = slot,
        .descriptorCount  = 1,
        .descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo       = &imageInfo,
        .pBufferInfo      = nullptr,
        .pTexelBufferView = nullptr,
    };
    for (uint32 i = 0; i < ctx.maxFramesInFlight(); i++) {
        descriptorWrite.dstSet = ctx.descriptorSet(i).descriptorSet();
        vkUpdateDescriptorSets(ctx.device(), 1, &descriptorWrite, 0, nullptr);
    }

    return slot;
}

void ResourceManager::unbindTexture(uint32 slot) {
    if (slot == UINT32_MAX) {
        return;
    }

    R3_ASSERT(m_textureBindSlots[slot].first > 0, "Assert valid operation");

    if ((--m_textureBindSlots[slot].first) == 0) {
        m_textureBindMap.erase(m_textureBindSlots[slot].second);
        m_textureFreeBindSlots.push_back(slot);
    }
}

void ResourceManager::clear() {
    m_bufferCache.clear();
    m_imageCache.clear();
    m_textureCache.clear();
    m_textureBindMap.clear();
    m_textureBindSlots.clear();
    m_textureFreeBindSlots.clear();
}

} // namespace R3