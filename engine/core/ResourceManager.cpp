#include "ResourceManager.hpp"

#if !R3_VULKAN
#error "Vulkan is currently the only supported RenderContext"
#else
#include <vulkan/vulkan_core.h>
#include "render/vulkan/vulkan-Buffer.hpp"
#include "render/vulkan/vulkan-CommandBuffer.hpp"
#include "render/vulkan/vulkan-RenderContext.hpp"
#include "render/vulkan/vulkan-Texture.hpp"
#endif

#include <span>
#include <utility>
#include <vector>
#include "api/Assert.hpp"
#include "api/Types.hpp"
#include "render/Flags.hpp"
#include "render/ShaderObjects.hpp"

namespace R3 {

template <typename T>
struct ResourceTracker {
    std::vector<T> data;
    std::vector<usize> freeSlots;

    template <typename... Args>
    usize add(Args&&... args) {
        usize index;
        if (freeSlots.empty()) {
            index = data.size();
            data.emplace_back(std::forward<Args>(args)...);
        } else {
            index = freeSlots.back();
            freeSlots.pop_back();
            data[index] = std::move(T{std::forward<Args>(args)...});
        }
        return index;
    }

    void remove(usize index) {
        R3_ASSERT(index < data.size() && "Index out of bounds");
        freeSlots.push_back(index);
    }

    void clear() {
        data.clear();
        freeSlots.clear();
    }
};

static ResourceTracker<vulkan::Buffer> s_vbos;
static ResourceTracker<vulkan::Buffer> s_ibos;
static ResourceTracker<vulkan::Texture> s_textures;

usize ResourceManagerSingleton::createVertexBuffer(std::span<const Vertex> vertices) {
    vulkan::RenderContext& ctx = *static_cast<vulkan::RenderContext*>(m_ctx);

    vulkan::Buffer vertexStagingBuffer = {
        ctx,
        vertices.size_bytes(),
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    };
    vertexStagingBuffer.copy(vertices);

    vulkan::Buffer vertexBuffer = {
        ctx,
        vertices.size_bytes(),
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    };

    const VkBufferCopy vertexCopyRegion = {
        .srcOffset = 0,
        .dstOffset = 0,
        .size      = vertices.size_bytes(),
    };

    vulkan::CommandBuffer& cmd = ctx.graphicsCommandBuffer(0);
    cmd.begin();
    cmd.copyBuffer(vertexStagingBuffer.buffer(), vertexBuffer.buffer(), {&vertexCopyRegion, 1});
    cmd.end();
    cmd.submitSync(ctx.graphicsQueue());

    return s_vbos.add(std::move(vertexBuffer));
}

void* ResourceManagerSingleton::vertexBufferAt(usize index) noexcept {
    if (index >= s_vbos.data.size()) {
        return nullptr;
    }
    return s_vbos.data[index].buffer();
}

usize ResourceManagerSingleton::createIndexBuffer(std::span<const uint32> indices) {
    vulkan::RenderContext& ctx = *static_cast<vulkan::RenderContext*>(m_ctx);

    vulkan::Buffer indexStagingBuffer = {
        ctx,
        indices.size_bytes(),
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    };
    indexStagingBuffer.copy(indices);

    vulkan::Buffer indexBuffer = {
        ctx,
        indices.size_bytes(),
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    };

    const VkBufferCopy indexCopyRegion = {
        .srcOffset = 0,
        .dstOffset = 0,
        .size      = indices.size_bytes(),
    };

    vulkan::CommandBuffer& cmd = ctx.graphicsCommandBuffer(0);
    cmd.begin();
    cmd.copyBuffer(indexStagingBuffer.buffer(), indexBuffer.buffer(), {&indexCopyRegion, 1});
    cmd.end();
    cmd.submitSync(ctx.graphicsQueue());

    return s_ibos.add(std::move(indexBuffer));
}

void* ResourceManagerSingleton::indexBufferAt(usize index) noexcept {
    if (index >= s_ibos.data.size()) {
        return nullptr;
    }
    return s_ibos.data[index].buffer();
}

usize ResourceManagerSingleton::createTexture(const uint8* raw, usize width, usize height, TextureType type) {
    vulkan::RenderContext& ctx = *static_cast<vulkan::RenderContext*>(m_ctx);
    vulkan::CommandBuffer& cmd = ctx.graphicsCommandBuffer(0);
    cmd.begin();
    usize index = s_textures.add(ctx, cmd, raw, width, height, type);
    cmd.end();
    cmd.submitSync(ctx.graphicsQueue());
    return index;
}

usize ResourceManagerSingleton::createTexture(const uint8* compressed, usize size, TextureType type) {
    vulkan::RenderContext& ctx = *static_cast<vulkan::RenderContext*>(m_ctx);
    vulkan::CommandBuffer& cmd = ctx.graphicsCommandBuffer(0);
    cmd.begin();
    usize index = s_textures.add(ctx, cmd, compressed, size, type);
    cmd.end();
    cmd.submitSync(ctx.graphicsQueue());
    return index;
}

usize ResourceManagerSingleton::createTexture(const std::filesystem::path& filepath, TextureType type) {
    vulkan::RenderContext& ctx = *static_cast<vulkan::RenderContext*>(m_ctx);
    vulkan::CommandBuffer& cmd = ctx.graphicsCommandBuffer(0);
    cmd.begin();
    usize index = s_textures.add(ctx, cmd, filepath, type);
    cmd.end();
    cmd.submitSync(ctx.graphicsQueue());
    return index;
}

void* ResourceManagerSingleton::textureAt(usize index) noexcept {
    if (index >= s_textures.data.size()) {
        return nullptr;
    }
    return s_textures.data[index].imageView(); // ??
}

void ResourceManagerSingleton::free() noexcept {
    s_vbos.clear();
    s_ibos.clear();
    s_textures.clear();
}

} // namespace R3