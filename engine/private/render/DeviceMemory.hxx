#pragma once

/// @brief Base for classes that need to allocate GPU memory

#include "NativeRenderObject.hxx"

namespace R3 {

/// @brief DeviceMemory base class
/// derived classes can store both their own Handle and a Memory handle (see VertexBuffer for example implementation)
/// it holds a Handle to the VAO itself and a handle to the device memory in the buffer
/// @note DeviceMemory does not handle clean up in anyway as it does not know implementation details
class DeviceMemory : public NativeRenderObject {
protected:
    DEFAULT_CONSTRUCT(DeviceMemory);
    NO_COPY(DeviceMemory);
    DEFAULT_MOVE(DeviceMemory);

    /// @brief Query is device memory is null
    /// @return true if null
    [[nodiscard]] constexpr bool validDeviceMemory() const { return m_deviceMemory != nullptr; }

    /// @brief Retrieve opaque buffer const handle
    /// @tparam T cast to T type
    /// @return buffer handle
    template <typename T>
    [[nodiscard]] constexpr T deviceMemory() const {
        return reinterpret_cast<T>(m_deviceMemory.get());
    }

    /// @brief Return buffer as a new type, used for C -> C++ bindings
    /// @tparam T cast to T type
    /// @return Type T Object from Buffer::Handle
    template <typename T>
    [[nodiscard]] constexpr T deviceMemoryAs() const {
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