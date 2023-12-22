#include "ResourceManager.hxx"

namespace R3 {

ResourceManager& GlobalResourceManager() {
    static ResourceManager s_resourceManager;
    return s_resourceManager;
}

TextureBuffer::ID ResourceManager::allocateTexture(const TextureBufferSpecification& spec) {
    if (m_texturePool.freeList.empty()) {
        m_texturePool.resources.emplace_back(spec);
        return m_texturePool.resources.size() - 1;
    } else {
        auto it = m_texturePool.freeList.begin();
        usize i = *it;

        m_texturePool.resources[i].~TextureBuffer();
        m_texturePool.resources[i] = TextureBuffer(spec);

        m_texturePool.freeList.erase(it);
        return i;
    }
}

void ResourceManager::freeTexture(TextureBuffer::ID index) {
    m_texturePool.freeList.insert(index);
}

TextureBuffer& ResourceManager::getTextureById(TextureBuffer::ID id) {
    return m_texturePool.resources[id];
}

UniformBuffer::ID ResourceManager::allocateUniform(const UniformBufferSpecification& spec) {
    if (m_uniformPool.freeList.empty()) {
        m_uniformPool.resources.emplace_back(spec);
        return m_uniformPool.resources.size() - 1;
    } else {
        auto it = m_uniformPool.freeList.begin();
        usize i = *it;

        m_uniformPool.resources[i].~UniformBuffer();
        m_uniformPool.resources[i] = UniformBuffer(spec);

        m_uniformPool.freeList.erase(it);
        return i;
    }
}

void ResourceManager::freeUniform(UniformBuffer::ID index) {
    m_uniformPool.freeList.insert(index);
}

UniformBuffer& ResourceManager::getUniformById(UniformBuffer::ID id) {
    return m_uniformPool.resources[id];
}

VertexBuffer::ID ResourceManager::allocateVertexBuffer(const VertexBufferSpecification& spec) {
    if (m_vertexPool.freeList.empty()) {
        m_vertexPool.resources.emplace_back(spec);
        return m_vertexPool.resources.size() - 1;
    } else {
        auto it = m_vertexPool.freeList.begin();
        usize i = *it;

        m_vertexPool.resources[i].~VertexBuffer();
        m_vertexPool.resources[i] = VertexBuffer(spec);

        m_vertexPool.freeList.erase(it);
        return i;
    }
}

void ResourceManager::freeVertexBuffer(VertexBuffer::ID index) {
    m_vertexPool.freeList.insert(index);
}

VertexBuffer& ResourceManager::getVertexBufferById(VertexBuffer::ID id) {
    return m_vertexPool.resources[id];
}

IndexBuffer<uint32>::ID ResourceManager::allocateIndexBuffer(const IndexBufferSpecification<uint32>& spec) {
    if (m_indexPool.freeList.empty()) {
        m_indexPool.resources.emplace_back(spec);
        return m_indexPool.resources.size() - 1;
    } else {
        auto it = m_indexPool.freeList.begin();
        usize i = *it;

        m_indexPool.resources[i].~IndexBuffer();
        m_indexPool.resources[i] = IndexBuffer(spec);

        m_indexPool.freeList.erase(it);
        return i;
    }
}

void ResourceManager::freeIndexBuffer(IndexBuffer<uint32>::ID index) {
    m_indexPool.freeList.insert(index);
}

IndexBuffer<uint32>& ResourceManager::getIndexBufferById(IndexBuffer<uint32>::ID id) {
    return m_indexPool.resources[id];
}

DescriptorPool::ID ResourceManager::allocateDescriptorPool(const DescriptorPoolSpecification& spec) {
    if (m_descriptorPool.freeList.empty()) {
        m_descriptorPool.resources.emplace_back(spec);
        return m_descriptorPool.resources.size() - 1;
    } else {
        auto it = m_descriptorPool.freeList.begin();
        usize i = *it;

        m_descriptorPool.resources[i].~DescriptorPool();
        m_descriptorPool.resources[i] = DescriptorPool(spec);

        m_descriptorPool.freeList.erase(it);
        return i;
    }
}

void ResourceManager::freeDescriptorPool(DescriptorPool::ID index) {
    m_descriptorPool.freeList.insert(index);
}

DescriptorPool& ResourceManager::getDescriptorPoolById(DescriptorPool::ID id) {
    return m_descriptorPool.resources[id];
}

GraphicsPipeline::ID ResourceManager::allocateGraphicsPipeline(const GraphicsPipelineSpecification& spec) {
    if (m_graphicsPipelinePool.freeList.empty()) {
        m_graphicsPipelinePool.resources.emplace_back(spec);
        return m_graphicsPipelinePool.resources.size() - 1;
    } else {
        auto it = m_graphicsPipelinePool.freeList.begin();
        usize i = *it;

        m_graphicsPipelinePool.resources[i].~GraphicsPipeline();
        m_graphicsPipelinePool.resources[i] = GraphicsPipeline(spec);

        m_graphicsPipelinePool.freeList.erase(it);
        return i;
    }
}

void ResourceManager::freeGraphicsPipeline(GraphicsPipeline::ID index) {
    m_graphicsPipelinePool.freeList.insert(index);
}

GraphicsPipeline& ResourceManager::getGraphicsPipelineById(GraphicsPipeline::ID id) {
    return m_graphicsPipelinePool.resources[id];
}

} // namespace R3