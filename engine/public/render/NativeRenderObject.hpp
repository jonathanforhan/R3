#pragma once
#include <type_traits>

namespace R3 {

/// @brief R3 Renderer uses opaque handles to wrap all implementation specific handles
/// for example a VkInstance will be wrapped in a NativeRenderObject:Instance
/// @note NativeRenderObject does not handle clean up in anyway as it does not know implementation details
class NativeRenderObject {
protected:
    NativeRenderObject() = default;

public:
    /// @brief Opaque handle type will be converted to API specific handle when queried
    using Handle = void*;

    /// @brief Retrieve opaque handle
    /// @tparam T cast to T type
    /// @return Handle
    template <typename T = Handle>
    constexpr T handle() {
        return reinterpret_cast<T>(m_handle);
    }

    /// @brief Retrieve opaque const handle
    /// @tparam T cast to T type
    /// @return Handle
    template <typename T = Handle>
    constexpr T handle() const {
        return reinterpret_cast<T>(m_handle);
    }

    /// @brief Return handle as a new type, used for C -> C++ bindings
    /// @tparam T cast to T type
    /// @return Type T Object from Handle
#if R3_VULKAN
    template <typename T>
    constexpr T as() {
        return T(reinterpret_cast<T::NativeType>(m_handle));
    }
    template <typename T>
    constexpr T as() const {
        return T(reinterpret_cast<const T::NativeType>(m_handle));
    }
#else
    template <typename T>
    constexpr T as() {
        return T(m_handle);
    }
    template <typename T> const
    constexpr T as() const {
        return T(m_handle);
    }
#endif


protected:
    /// @brief Retrieve opaque handle pointer, often used to set handle value
    /// @tparam T cast to T type
    /// @return Handle pointer
    template <typename T = Handle*>
    constexpr T handlePtr() {
        return reinterpret_cast<T>(&m_handle);
    }

    /// @brief Set handle
    /// @param handle 
    void setHandle(Handle handle) { m_handle = handle; }

private:
    Handle m_handle;
};

} // namespace R3