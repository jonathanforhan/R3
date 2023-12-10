#pragma once
#include "api/Ref.hpp"

namespace R3 {

/// @brief R3 Renderer uses opaque handles to wrap all implementation specific handles
/// for example a VkInstance will be wrapped in a NativeRenderObject:Instance
/// @note NativeRenderObject does not handle clean up in anyway as it does not know implementation details
class NativeRenderObject {
protected:
    NativeRenderObject() = default;

public:
    NativeRenderObject(const NativeRenderObject&) = delete;
    NativeRenderObject& operator=(const NativeRenderObject&) = delete;

    NativeRenderObject(NativeRenderObject&&) R3_NOEXCEPT = default;
    NativeRenderObject& operator=(NativeRenderObject&&) R3_NOEXCEPT = default;

    /// @brief Opaque handle type will be converted to API specific handle when queried
    using Handle = void*;

    /// @brief Query is handle is null
    /// @return true if not null
    [[nodiscard]] bool validHandle() const { return m_handle != nullptr; }

    /// @brief Retrieve opaque handle
    /// @tparam T cast to T type
    /// @return Handle
    template <typename T = Handle>
    constexpr T handle() {
        return reinterpret_cast<T>(m_handle.get());
    }

    /// @brief Retrieve opaque const handle
    /// @tparam T cast to T type
    /// @return Handle
    template <typename T = Handle>
    constexpr T handle() const {
        return reinterpret_cast<T>(m_handle.get());
    }

    /// @brief Return handle as a new type, used for C -> C++ bindings
    /// @tparam T cast to T type
    /// @return Type T Object from Handle
#if R3_VULKAN
    template <typename T>
    constexpr T as() {
        CHECK(validHandle());
        return T(reinterpret_cast<T::NativeType>(m_handle.get()));
    }
    template <typename T>
    constexpr T as() const {
        CHECK(validHandle());
        return T(reinterpret_cast<const T::NativeType>(m_handle.get()));
    }
#else
    template <typename T>
    constexpr T as() {
        CHECK(validHandle());
        return T(*m_handle);
    }
    template <typename T> const
    constexpr T as() const {
        CHECK(validHandle());
        return T(*m_handle);
    }
#endif

protected:
    /// @brief Set handle
    /// @param handle 
    void setHandle(Handle handle) { m_handle = handle; }

private:
    Ref<std::remove_pointer_t<Handle>> m_handle;
};

} // namespace R3