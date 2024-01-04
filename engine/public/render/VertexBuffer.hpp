#pragma once

#include <span>
#include "render/Buffer.hpp"
#include "render/RenderApi.hpp"
#include "render/Vertex.hpp"

namespace R3 {

/// @brief Vertex Buffer Specification
struct R3_API VertexBufferSpecification {
    const PhysicalDevice& physicalDevice; ///< PhysicalDevice
    const LogicalDevice& logicalDevice;   ///< LogicalDevice
    const CommandBuffer& commandBuffer;   ///< CommandBuffer
    std::span<const Vertex> vertices;     ///< Vertice data buffer
};

/// @brief VertexBuffer (VAO) holds serialized vertex data on Device
class R3_API VertexBuffer : public Buffer {
public:
    DEFAULT_CONSTRUCT(VertexBuffer);
    NO_COPY(VertexBuffer);
    DEFAULT_MOVE(VertexBuffer);

    /// @brief Construct VertexBuffer from spec
    /// @param spec
    VertexBuffer(const VertexBufferSpecification& spec);

    /// @brief Free VertexBuffer
    ~VertexBuffer();

    [[nodiscard]] constexpr uint32 count() const { return m_vertexCount; } ///< Query Vertex count

private:
    Ref<const LogicalDevice> m_logicalDevice;
    uint32 m_vertexCount = 0;
};

} // namespace R3