#pragma once
#include "api/Ref.hpp"
#include <concepts>

namespace R3 {

template <typename T>
concept IsWrapper = requires { typename T::NativeType; };

/// @brief R3 Renderer uses opaque handles to wrap all implementation specific handles
/// for example a VkInstance will be wrapped in a NativeRenderObject:Instance
/// @note NativeRenderObject does not handle clean up in anyway as it does not know implementation details
class NativeRenderObject {
public:
    /// @brief Opaque Handle type will be converted to API specific Handle when queried
    using Handle = void*;
    using HandleRef = Ref<std::remove_pointer_t<Handle>>;
    using HandleConstRef = Ref<const std::remove_pointer_t<Handle>>;

protected:
    NativeRenderObject() = default;

public:
    template <typename T = Handle>
    NativeRenderObject(const T& handle) {
        if constexpr (IsWrapper<T>) {
            m_handle = reinterpret_cast<Handle>(static_cast<T::NativeType>(handle));
        } else {
            m_handle = reinterpret_cast<Handle>(handle);
        }
    }

    NativeRenderObject(const NativeRenderObject&) = delete;
    NativeRenderObject& operator=(const NativeRenderObject&) = delete;

    NativeRenderObject(NativeRenderObject&&) R3_NOEXCEPT = default;
    NativeRenderObject& operator=(NativeRenderObject&&) R3_NOEXCEPT = default;

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
    template <typename T = Handle>
    void setHandle(const T& handle) {
        if constexpr (IsWrapper<T>) {
            m_handle = reinterpret_cast<Handle>(static_cast<T::NativeType>(handle));
        } else {
            m_handle = reinterpret_cast<Handle>(handle);
        }
    }

private:
    HandleRef m_handle;
};

} // namespace R3