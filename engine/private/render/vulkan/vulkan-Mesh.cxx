#if R3_VULKAN

#include "render/Mesh.hpp"

#include <vulkan/vulkan.hpp>

namespace R3 {

Mesh::Mesh(const MeshSpecification& spec)
    : m_spec(spec) {
    m_vertexBuffer = VertexBuffer(VertexBufferSpecification{
        .physicalDevice = m_spec.physicalDevice,
        .logicalDevice = m_spec.logicalDevice,
        .commandPool = m_spec.commandPool,
        .vertices = m_spec.vertices,
    });

    m_indexBuffer = IndexBuffer<uint32>(IndexBufferSpecification<uint32>{
        .physicalDevice = m_spec.physicalDevice,
        .logicalDevice = m_spec.logicalDevice,
        .commandPool = m_spec.commandPool,
        .indices = m_spec.indices,
    });
}

Mesh::~Mesh() {}

bool Mesh::removeTextureIndex(uint32 index) {
    auto it = std::ranges::find(m_textureIndices, index);
    bool foundIndex = it != m_textureIndices.end();
    if (foundIndex) {
        m_textureIndices.erase(it);
    }
    return foundIndex;
}

} // namespace R3

#endif // R3_VULKAN