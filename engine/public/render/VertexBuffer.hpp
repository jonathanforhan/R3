#pragma once

#include <span>
#include "render/Buffer.hpp"
#include "render/RenderFwd.hpp"
#include "render/Vertex.hpp"

namespace R3 {

struct VertexBufferSpecification {
    const PhysicalDevice& physicalDevice;
    const LogicalDevice& logicalDevice;
    const CommandPool& commandPool;
    std::span<const Vertex> vertices;
};

class VertexBuffer : public Buffer {
public:
    VertexBuffer() = default;
    VertexBuffer(const VertexBufferSpecification& spec);
    VertexBuffer(VertexBuffer&&) noexcept = default;
    VertexBuffer& operator=(VertexBuffer&&) noexcept = default;
    ~VertexBuffer();

    uint32 count() const { return m_vertexCount; }

private:
    Ref<const LogicalDevice> m_logicalDevice;
    uint32 m_vertexCount = 0;
};

} // namespace R3