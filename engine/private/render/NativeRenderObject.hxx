#pragma once

/// Wraps different Render API Handle

#include "api/Ref.hpp"
#include "render/RenderApi.hxx"

namespace R3 {

/// @brief Comp-time api wrapper query
template <typename T>
concept IsWrapper = requires { typename T::NativeType; };

/// @brief R3 Renderer uses opaque handles to wrap all implementation specific handles
/// for example a VkInstance will be wrapped in a NativeRenderObject:Instance
/// @note NativeRenderObject does not handle clean up in anyway as it does not know implementation details
class NativeRenderObject {
public:
    using Handle = void*; ///< Opaque Handle type will be converted to API specific Handle when queried
    using HandleRef = Ref<std::remove_pointer_t<Handle>>;            ///< Handle Reference Type
    using HandleConstRef = Ref<const std::remove_pointer_t<Handle>>; ///< Handle const Reference Type
    using ID = usize;                                                ///< ID type used by GlobalResourceManager

protected:
    DEFAULT_CONSTRUCT(NativeRenderObject);

public:
    NO_COPY(NativeRenderObject);
    DEFAULT_MOVE(NativeRenderObject);

    /// @brief Construct NativeRenderObject from handle
    /// If handle is API wrapper NRO behave accordingly
    /// @tparam T handle type
    /// @param handle
    template <typename T = Handle>
    NativeRenderObject(const T& handle) {
        if constexpr (IsWrapper<T>) {
            m_handle = reinterpret_cast<Handle>(static_cast<T::NativeType>(handle));
        } else {
            m_handle = reinterpret_cast<Handle>(handle);
        }
    }

    /// @brief Query is handle is null
    /// @return true if not null
    [[nodiscard]] constexpr bool validHandle() const { return m_handle != nullptr; }

    /// @brief Retrieve opaque handle
    /// @tparam T cast to T type
    /// @return Handle
    template <typename T = Handle>
    [[nodiscard]] constexpr T handle() const {
        return reinterpret_cast<T>(m_handle.get());
    }

    /// @brief Return handle as a new type, used for C -> C++ bindings
    /// @tparam T cast to T type
    /// @return Type T Object from Handle
    template <typename T>
    [[nodiscard]] constexpr T as() const {
        CHECK(validHandle());
        if constexpr (IsWrapper<T>) {
            return static_cast<T>(reinterpret_cast<T::NativeType>(m_handle.get()));
        } else {
            return static_cast<T>(m_handle.get());
        }
    }

protected:
    /// @brief Set handle
    /// @param handle
    template <typename T = Handle>
    constexpr void setHandle(const T& handle) {
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