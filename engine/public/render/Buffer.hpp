#pragma once

#include "render/NativeRenderObject.hpp"

namespace R3 {

/// @brief Buffer base class, derived classed can store both their own Handle and a Buffer handle
/// see VertexBuffer for example, holds a Handle to the VAO itself and a handle to the device memory in the buffer
/// @note Buffer does not handle clean up in anyway as it does not know implementation details
class Buffer : public NativeRenderObject {
protected:
    Buffer() = default;

    /// @brief Retrieve opaque buffer handle
    /// @tparam T cast to T type
    /// @return buffer handle
    template <typename T>
    T buffer() {
        return reinterpret_cast<T>(m_deviceMemory);
    }

    /// @brief Retrieve opaque buffer const handle
    /// @tparam T cast to T type
    /// @return buffer handle
    template <typename T>
    T buffer() const {
        return reinterpret_cast<T>(m_deviceMemory);
    }

    /// @brief Retrieve opaque buffer handle pointer
    /// @tparam T cast to T type
    /// @return buffer handle pointer
    template <typename T>
    T bufferPtr() {
        return reinterpret_cast<T>(&m_deviceMemory);
    }

    /// @brief Set Buffer handle
    /// @param bufferHandle 
    void setBuffer(Handle bufferHandle) { m_deviceMemory = bufferHandle; }

private:
    // points to device (GPU) memory allocated for buffer
    Handle m_deviceMemory;
};

} // namespace R3