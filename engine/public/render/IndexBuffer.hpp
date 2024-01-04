#pragma once

/// @brief Conatains Handle to DeviceMemory where data is stored

#include <span>
#include "render/Buffer.hpp"
#include "render/RenderApi.hpp"

namespace R3 {

/// @brief Index Buffer Specification
/// @tparam T IndexBuffer datatype
template <std::integral T>
struct R3_API IndexBufferSpecification {
    const PhysicalDevice& physicalDevice; ///< PhysicalDevice
    const LogicalDevice& logicalDevice;   ///< LogicalDevice
    const CommandBuffer& commandBuffer;   ///< CommandPool
    std::span<const T> indices;           ///< Array of indices
};

/// @brief IndexBuffers are used to reduce the repetition on draw commands
/// IndexBuffers are allocated by GlobalResourceManager
/// @tparam T IndexBuffer datatype
template <std::integral T>
class R3_API IndexBuffer : public Buffer {
public:
    DEFAULT_CONSTRUCT(IndexBuffer);
    NO_COPY(IndexBuffer);
    DEFAULT_MOVE(IndexBuffer);

    /// @brief Construct IndexBuffer of T type from spec
    /// @param spec
    IndexBuffer(const IndexBufferSpecification<T>& spec);

    /// @brief Free IndexBuffer
    ~IndexBuffer();

    /// @brief Number of indices
    /// @return count
    [[nodiscard]] constexpr uint32 count() const { return m_indexCount; }

private:
    Ref<const LogicalDevice> m_logicalDevice;
    uint32 m_indexCount = 0;
};

extern template class IndexBuffer<uint32>; ///< Forward Declaration
extern template class IndexBuffer<uint16>; ///< Forward Declaration

} // namespace R3
