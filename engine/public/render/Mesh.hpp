#pragma once

#include <span>
#include <string_view>
#include <vector>
#include "render/GraphicsPipeline.hpp"
#include "render/IndexBuffer.hpp"
#include "render/Material.hpp"
#include "render/VertexBuffer.hpp"

namespace R3 {

struct MeshSpecification {
    Ref<const PhysicalDevice> physicalDevice;
    Ref<const LogicalDevice> logicalDevice;
    Ref<const CommandPool> commandPool;
    const MaterialSpecification& materialSpecification;
    std::string_view vertexShaderPath;
    std::string_view fragmentShaderPath;
    std::span<const Vertex> vertices;
    std::span<const uint32> indices;
};

class Mesh {
public:
    Mesh() = default;

    Mesh(const MeshSpecification& spec);
    Mesh(const Mesh&) = delete;
    Mesh(Mesh&&) noexcept = default;
    Mesh& operator=(Mesh&&) noexcept = default;

    uint32 vertexCount() const { return m_vertexBuffer.count(); }
    const VertexBuffer& vertexBuffer() const { return m_vertexBuffer; }

    uint32 indexCount() const { return m_indexBuffer.count(); }
    const IndexBuffer<uint32>& indexBuffer() const { return m_indexBuffer; }

    Material& material() { return m_material; }

    GraphicsPipeline& pipeline() { return m_pipeline; }

private:
    VertexBuffer m_vertexBuffer;
    IndexBuffer<uint32> m_indexBuffer;
    Material m_material;
    GraphicsPipeline m_pipeline;
};

} // namespace R3
