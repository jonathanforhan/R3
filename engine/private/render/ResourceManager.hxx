#pragma once

#include <mutex>
#include <unordered_set>
#include "render/DescriptorPool.hpp"
#include "render/GraphicsPipeline.hpp"
#include "render/IndexBuffer.hpp"
#include "render/RenderApi.hpp"
#include "render/TextureBuffer.hpp"
#include "render/UniformBuffer.hpp"
#include "render/VertexBuffer.hpp"

namespace R3 {

template <typename T>
struct ResourcePool {
    std::vector<T> resources;
    std::unordered_set<usize> freeList;
    std::mutex mutex;
};

class ResourceManager {
public:
    DEFAULT_CONSTRUCT(ResourceManager);
    NO_COPY(ResourceManager);
    NO_MOVE(ResourceManager);

    [[nodiscard]] TextureBuffer::ID allocateTexture(const TextureBufferSpecification& spec);
    void freeTexture(TextureBuffer::ID index);
    [[nodiscard]] TextureBuffer& getTextureById(TextureBuffer::ID id);

    [[nodiscard]] UniformBuffer::ID allocateUniform(const UniformBufferSpecification& spec);
    void freeUniform(UniformBuffer::ID index);
    [[nodiscard]] UniformBuffer& getUniformById(UniformBuffer::ID id);

    [[nodiscard]] StorageBuffer::ID allocateStorageBuffer(const StorageBufferSpecification& spec);
    void freeStorageBuffer(StorageBuffer::ID index);
    [[nodiscard]] StorageBuffer& getStorageBufferById(StorageBuffer::ID id);

    [[nodiscard]] VertexBuffer::ID allocateVertexBuffer(const VertexBufferSpecification& spec);
    void freeVertexBuffer(VertexBuffer::ID index);
    [[nodiscard]] VertexBuffer& getVertexBufferById(VertexBuffer::ID id);

    [[nodiscard]] IndexBuffer<uint32>::ID allocateIndexBuffer(const IndexBufferSpecification<uint32>& spec);
    void freeIndexBuffer(IndexBuffer<uint32>::ID index);
    [[nodiscard]] IndexBuffer<uint32>& getIndexBufferById(IndexBuffer<uint32>::ID id);

    [[nodiscard]] DescriptorPool::ID allocateDescriptorPool(const DescriptorPoolSpecification& spec);
    void freeDescriptorPool(DescriptorPool::ID index);
    [[nodiscard]] DescriptorPool& getDescriptorPoolById(DescriptorPool::ID id);

    [[nodiscard]] GraphicsPipeline::ID allocateGraphicsPipeline(const GraphicsPipelineSpecification& spec);
    void freeGraphicsPipeline(GraphicsPipeline::ID index);
    [[nodiscard]] GraphicsPipeline& getGraphicsPipelineById(GraphicsPipeline::ID id);

private:
    ResourcePool<TextureBuffer> m_texturePool;
    ResourcePool<UniformBuffer> m_uniformPool;
    ResourcePool<StorageBuffer> m_storagePool;
    ResourcePool<VertexBuffer> m_vertexPool;
    ResourcePool<IndexBuffer<uint32>> m_indexPool;
    ResourcePool<DescriptorPool> m_descriptorPool;
    ResourcePool<GraphicsPipeline> m_graphicsPipelinePool;
};

} // namespace R3
