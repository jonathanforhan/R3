#pragma once

#include <span>
#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"
#include "render/Vertex.hpp"
#include "render/Buffer.hpp"

namespace R3 {

struct VertexBufferSpecification {
    const PhysicalDevice* physicalDevice;
    const LogicalDevice* logicalDevice;
    std::span<Vertex> vertices;
};

class VertexBuffer : public Buffer {
public:
    void create(const VertexBufferSpecification& spec);
    void destroy();

    uint32 vertexCount() const { return static_cast<uint32>(m_spec.vertices.size()); }

private:
    VertexBufferSpecification m_spec;
};

} // namespace R3