#pragma once

#include <unordered_set>
#include <vector>
#include "render/DescriptorPool.hpp"
#include "render/GraphicsPipeline.hpp"
#include "render/IndexBuffer.hpp"
#include "render/RenderApi.hpp"
#include "render/TextureBuffer.hpp"
#include "render/UniformBuffer.hpp"
#include "render/VertexBuffer.hpp"

namespace R3 {

class ResourceManager;

template <typename T>
struct ResourcePool {
    std::vector<T> resources;
    std::unordered_set<usize> freeList;
};

extern ResourceManager GlobalResourceManager;

class ResourceManager {
public:
    DEFAULT_CONSTRUCT(ResourceManager);
    NO_COPY(ResourceManager);
    NO_MOVE(ResourceManager);

    TextureBuffer::ID allocateTexture(const TextureBufferSpecification& spec);
    void freeTexture(TextureBuffer::ID index);
    TextureBuffer& getTextureById(TextureBuffer::ID id);

    UniformBuffer::ID allocateUniform(const UniformBufferSpecification& spec);
    void freeUniform(UniformBuffer::ID index);
    UniformBuffer& getUniformById(UniformBuffer::ID id);

    VertexBuffer::ID allocateVertexBuffer(const VertexBufferSpecification& spec);
    void freeVertexBuffer(VertexBuffer::ID index);
    VertexBuffer& getVertexBufferById(VertexBuffer::ID id);

    IndexBuffer<uint32>::ID allocateIndexBuffer(const IndexBufferSpecification<uint32>& spec);
    void freeIndexBuffer(IndexBuffer<uint32>::ID index);
    IndexBuffer<uint32>& getIndexBufferById(IndexBuffer<uint32>::ID id);

    DescriptorPool::ID allocateDescriptorPool(const DescriptorPoolSpecification& spec);
    void freeDescriptorPool(DescriptorPool::ID index);
    DescriptorPool& getDescriptorPoolById(DescriptorPool::ID id);

    GraphicsPipeline::ID allocateGraphicsPipeline(const GraphicsPipelineSpecification& spec);
    void freeGraphicsPipeline(GraphicsPipeline::ID index);
    GraphicsPipeline& getGraphicsPipelineById(GraphicsPipeline::ID id);

private:
    ResourcePool<TextureBuffer> m_texturePool;
    ResourcePool<UniformBuffer> m_uniformPool;
    ResourcePool<VertexBuffer> m_vertexPool;
    ResourcePool<IndexBuffer<uint32>> m_indexPool;
    ResourcePool<DescriptorPool> m_descriptorPool;
    ResourcePool<GraphicsPipeline> m_graphicsPipelinePool;
};

} // namespace R3
