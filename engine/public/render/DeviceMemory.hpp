#pragma once

#include "render/NativeRenderObject.hpp"

namespace R3 {

/// @brief Device base class, derived classed can store both their own Handle and a Memory handle
/// see VertexBuffer for example, holds a Handle to the VAO itself and a handle to the device memory in the buffer
/// DeviceMemory means GPU memory
/// @note DeviceMemory does not handle clean up in anyway as it does not know implementation details
class DeviceMemory : public NativeRenderObject {
protected:
    DeviceMemory() = default;
    DeviceMemory(const DeviceMemory&) = delete;
    DeviceMemory& operator=(const DeviceMemory&) = delete;

    DeviceMemory(DeviceMemory&& src) R3_NOEXCEPT = default;
    DeviceMemory& operator=(DeviceMemory&& src) R3_NOEXCEPT = default;

    /// @brief Query is device memory is null
    /// @return true if null
    [[nodiscard]] constexpr bool validDeviceMemory() const { return m_deviceMemory != nullptr; }

    /// @brief Retrieve opaque buffer handle
    /// @tparam T cast to T type
    /// @return buffer handle
    template <typename T>
    constexpr T deviceMemory() {
        return reinterpret_cast<T>(m_deviceMemory.get());
    }

    /// @brief Retrieve opaque buffer const handle
    /// @tparam T cast to T type
    /// @return buffer handle
    template <typename T>
    constexpr T deviceMemory() const {
        return reinterpret_cast<T>(m_deviceMemory.get());
    }

    /// @brief Return buffer as a new type, used for C -> C++ bindings
    /// @tparam T cast to T type
    /// @return Type T Object from Buffer::Handle
    template <typename T>
    constexpr T deviceMemoryAs() {
        if constexpr (IsWrapper<T>) {
            return static_cast<T>(reinterpret_cast<T::NativeType>(m_deviceMemory.get()));
        } else {
            return static_cast<T>(m_deviceMemory.get());
        }
    }

    template <typename T>
    constexpr T deviceMemoryAs() const {
        if constexpr (IsWrapper<T>) {
            return static_cast<T>(reinterpret_cast<T::NativeType>(m_deviceMemory.get()));
        } else {
            return static_cast<T>(m_deviceMemory.get());
        }
    }

    /// @brief Set Buffer handle
    /// @param bufferHandle 
    template <typename T = Handle>
    constexpr void setDeviceMemory(const T& bufferHandle) {
        if constexpr (IsWrapper<T>) {
            m_deviceMemory = reinterpret_cast<Handle>(static_cast<T::NativeType>(bufferHandle));
        } else {
            m_deviceMemory = reinterpret_cast<Handle>(bufferHandle);
        }
    }

private:
    // points to device (GPU) memory allocated for buffer
    HandleRef m_deviceMemory;
};

} // namespace R3