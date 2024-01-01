#pragma once

#include "render/Buffer.hxx"
#include "render/RenderApi.hxx"

namespace R3 {

/// @brief Uniform Buffer Specification
struct UniformBufferSpecification {
    const PhysicalDevice& physicalDevice; ///< PhysicalDevice
    const LogicalDevice& logicalDevice;   ///< LogicalDevice
    usize bufferSize;                     ///< Buffer size in bytes
};

/// @brief Buffer of Shader Uniform data
class UniformBuffer : public Buffer {
public:
    DEFAULT_CONSTRUCT(UniformBuffer);
    NO_COPY(UniformBuffer);
    DEFAULT_MOVE(UniformBuffer);

    /// @brief Construct UniformBuffer from spec
    /// @param spec
    UniformBuffer(const UniformBufferSpecification& spec);

    /// @brief Free UniformBuffer
    ~UniformBuffer();

    /// @brief Write data to UniformBuffer
    /// @param data Buffer of data to write
    /// @param size Size of data in bytes
    /// @param offset Offset of UniformBuffer to write to
    void update(const void* data, usize size, usize offset = 0);

private:
    Ref<const LogicalDevice> m_logicalDevice;
    void* m_mappedMemory = nullptr;
    usize m_bufferSize;
};

} // namespace R3
