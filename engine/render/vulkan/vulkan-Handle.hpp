#pragma once

#include <type_traits>
#include <utility>
#include "api/Class.hpp"

namespace R3::vulkan {

/// @brief Wrapper for Vulkan handles with move semantics
/// @tparam T Vulkan handle type (e.g., VkBuffer, VkImage, VkDeviceMemory)
template <typename T>
requires std::is_pointer_v<T>
class Handle {
public:
    R3_COPY_DELETE(Handle);

    /// Constructor with handle value, defaults to nullptr
    Handle(T handle = nullptr) noexcept
        : m_handle(handle) {}

    /// Move constructor - transfers ownership, leaves source as nullptr
    Handle(Handle&& other) noexcept
        : m_handle(std::exchange(other.m_handle, nullptr)) {}

    /// Move assignment operator - transfers ownership, leaves source as nullptr
    Handle& operator=(Handle&& other) noexcept {
        if (this != &other) {
            m_handle = std::exchange(other.m_handle, m_handle);
        }
        return *this;
    }

    /// Implicit conversion to underlying handle type for convenience
    operator T&() noexcept { return m_handle; }
    operator const T&() const noexcept { return m_handle; }
    T& operator*() noexcept { return m_handle; }
    const T& operator*() const noexcept { return m_handle; }

private:
    T m_handle;
};

} // namespace R3::vulkan
