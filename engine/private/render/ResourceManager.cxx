#include "ResourceManager.hxx"

namespace R3 {

namespace local {

template <typename T, typename S>
auto allocate(ResourcePool<T>& resourcePool, const S& spec) -> T::ID {
    static_assert(std::is_base_of_v<NativeRenderObject, T>);
    static_assert(std::is_move_assignable_v<T>);
    static_assert(not std::is_copy_assignable_v<T>);

    auto resource = T(spec);

    std::scoped_lock<std::mutex> lock(resourcePool.mutex);

    if (resourcePool.freeList.empty()) {
        resourcePool.resources.emplace_back(std::move(resource));
        return resourcePool.resources.size() - 1;
    } else {
        auto it = resourcePool.freeList.begin();
        usize i = *it;

        resourcePool.resources[i].~T();
        resourcePool.resources[i] = std::move(resource);

        resourcePool.freeList.erase(it);
        return i;
    }
}

} // namespace local

TextureBuffer::ID ResourceManager::allocateTexture(const TextureBufferSpecification& spec) {
    return local::allocate(m_texturePool, spec);
}

void ResourceManager::freeTexture(TextureBuffer::ID index) {
    m_texturePool.freeList.insert(index);
}

TextureBuffer& ResourceManager::getTextureById(TextureBuffer::ID id) {
    return m_texturePool.resources[id];
}

UniformBuffer::ID ResourceManager::allocateUniform(const UniformBufferSpecification& spec) {
    return local::allocate(m_uniformPool, spec);
}

void ResourceManager::freeUniform(UniformBuffer::ID index) {
    m_uniformPool.freeList.insert(index);
}

UniformBuffer& ResourceManager::getUniformById(UniformBuffer::ID id) {
    return m_uniformPool.resources[id];
}

VertexBuffer::ID ResourceManager::allocateVertexBuffer(const VertexBufferSpecification& spec) {
    return local::allocate(m_vertexPool, spec);
}

void ResourceManager::freeVertexBuffer(VertexBuffer::ID index) {
    m_vertexPool.freeList.insert(index);
}

VertexBuffer& ResourceManager::getVertexBufferById(VertexBuffer::ID id) {
    return m_vertexPool.resources[id];
}

IndexBuffer<uint32>::ID ResourceManager::allocateIndexBuffer(const IndexBufferSpecification<uint32>& spec) {
    return local::allocate(m_indexPool, spec);
}

void ResourceManager::freeIndexBuffer(IndexBuffer<uint32>::ID index) {
    m_indexPool.freeList.insert(index);
}

IndexBuffer<uint32>& ResourceManager::getIndexBufferById(IndexBuffer<uint32>::ID id) {
    return m_indexPool.resources[id];
}

DescriptorPool::ID ResourceManager::allocateDescriptorPool(const DescriptorPoolSpecification& spec) {
    return local::allocate(m_descriptorPool, spec);
}

void ResourceManager::freeDescriptorPool(DescriptorPool::ID index) {
    m_descriptorPool.freeList.insert(index);
}

DescriptorPool& ResourceManager::getDescriptorPoolById(DescriptorPool::ID id) {
    return m_descriptorPool.resources[id];
}

GraphicsPipeline::ID ResourceManager::allocateGraphicsPipeline(const GraphicsPipelineSpecification& spec) {
    return local::allocate(m_graphicsPipelinePool, spec);
}

void ResourceManager::freeGraphicsPipeline(GraphicsPipeline::ID index) {
    m_graphicsPipelinePool.freeList.insert(index);
}

GraphicsPipeline& ResourceManager::getGraphicsPipelineById(GraphicsPipeline::ID id) {
    return m_graphicsPipelinePool.resources[id];
}

} // namespace R3