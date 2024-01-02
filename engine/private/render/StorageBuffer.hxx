#pragma once

#include "render/Buffer.hxx"
#include "render/RenderApi.hxx"

namespace R3 {

/// @brief Storage Buffer Specification
struct StorageBufferSpecification {
    const PhysicalDevice& physicalDevice; ///< PhysicalDevice
    const LogicalDevice& logicalDevice;   ///< LogicalDevice
    usize bufferSize;                     ///< Buffer size in bytes
};

/// @brief Buffer of Shader Storage data
class StorageBuffer : public Buffer {
public:
    DEFAULT_CONSTRUCT(StorageBuffer);
    NO_COPY(StorageBuffer);
    DEFAULT_MOVE(StorageBuffer);

    /// @brief Construct StorageBuffer from spec
    /// @param spec
    StorageBuffer(const StorageBufferSpecification& spec);

    /// @brief Free StorageBuffer
    ~StorageBuffer();

    /// @brief Write data to StorageBuffer
    /// @param data Buffer of data to write
    /// @param size Size of data in bytes
    /// @param offset Offset of StorageBuffer to write to
    void write(const void* data, usize size, usize offset);

    template <std::integral T>
    [[nodiscard]] const T* read() const {
        return static_cast<T*>(m_mappedMemory);
    }

private:
    Ref<const LogicalDevice> m_logicalDevice;
    usize m_bufferSize = 0;
    void* m_mappedMemory = nullptr;
};

} // namespace R3