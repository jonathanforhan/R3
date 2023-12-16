#pragma once

#include <span>
#include <vector>
#include "render/IndexBuffer.hpp"
#include "render/Vertex.hpp"
#include "render/VertexBuffer.hpp"

namespace R3 {

struct MeshSpecification {
    Ref<const PhysicalDevice> physicalDevice;
    Ref<const LogicalDevice> logicalDevice;
    Ref<const CommandPool> commandPool;
    std::span<const Vertex> vertices;
    std::span<const uint32> indices;
};

class Mesh {
public:
    Mesh() = default;

    Mesh(const MeshSpecification& spec);
    Mesh(Mesh&&) noexcept = default;
    Mesh& operator=(Mesh&&) noexcept = default;
    ~Mesh();

    uint32 vertexCount() const { return m_vertexBuffer.count(); }
    const VertexBuffer& vertexBuffer() const { return m_vertexBuffer; }

    uint32 indexCount() const { return m_indexBuffer.count(); }
    const IndexBuffer<uint32>& indexBuffer() const { return m_indexBuffer; }

    void addTextureIndex(uint32 index) { m_textureIndices.push_back(index); }
    bool removeTextureIndex(uint32 index); // return true if index was erased

private:
    MeshSpecification m_spec;
    VertexBuffer m_vertexBuffer;
    IndexBuffer<uint32> m_indexBuffer;
    std::vector<uint32> m_textureIndices;
};

} // namespace R3
