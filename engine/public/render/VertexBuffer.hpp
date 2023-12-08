#pragma once

#include <span>
#include "render/Buffer.hpp"
#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"
#include "render/Vertex.hpp"

namespace R3 {

struct VertexBufferSpecification {
    const PhysicalDevice* physicalDevice;
    const LogicalDevice* logicalDevice;
    const CommandPool* commandPool;
    std::span<Vertex> vertices;
};

class VertexBuffer : public Buffer {
public:
    void create(const VertexBufferSpecification& spec);
    void destroy();

    uint32 vertexCount() const { return m_vertexCount; }

private:
    VertexBufferSpecification m_spec;
    uint32 m_vertexCount = 0;
};

} // namespace R3