#pragma once

#include <type_traits>
#include <utility>
#include "engine/api/Api.hpp"
#include "engine/api/Class.hpp"

namespace R3 {

/// @brief Wrapper for API handles with move semantics
/// @tparam T Resource handle type (e.g., VkBuffer, VkImage, VkDeviceMemory, GLFWwindow)
/// Sets the handle to 0 (NULL) on move, preventing double-free errors (when the destructor checks)
template <typename T>
requires std::is_pointer_v<T> || std::is_integral_v<T>
class R3_API MovableHandle {
public:
    R3_COPY_DELETE(MovableHandle);

    /// Constructor with default handle value
    MovableHandle(T handle = {}) noexcept
        : m_handle(handle) {}

    /// @brief Takes ownership of the handle from another MovableHandle instance.
    /// @param other The MovableHandle instance to move from. After the operation, other will no longer own the handle.
    MovableHandle(MovableHandle&& other) noexcept
        : m_handle(std::exchange(other.m_handle, nullptr)) {}

    /// @brief Swaps ownership of the handle from another MovableHandle instance with this instance.
    /// @param other The MovableHandle instance to swap with.
    /// @return A reference to this MovableHandle instance after the move assignment.
    MovableHandle& operator=(MovableHandle&& other) noexcept {
        if (this != &other) {
            m_handle = std::exchange(other.m_handle, m_handle);
        }
        return *this;
    }

    /// @brief Implicitly returns a reference to the object managed by the handle.
    operator T&() noexcept { return m_handle; }

    /// @brief Implicitly returns a constant reference to the object managed by the handle.
    operator const T&() const noexcept { return m_handle; }

    /// @brief Returns a reference to the object managed by the handle.
    /// @return A reference to the managed object.
    T& operator*() noexcept { return m_handle; }

    /// @brief Returns a constant reference to the object managed by the handle.
    /// @return A constant reference to the managed object.
    const T& operator*() const noexcept { return m_handle; }

    /// @brief Returns a reference to the object managed by the handle.
    /// @tparam U Type to cast the handle to (e.g., VkBuffer, VkImage, VkDeviceMemory, GLFWwindow)
    /// @return A reference to the managed object.
    template <typename U = T>
    U& get() noexcept {
        return *reinterpret_cast<U*>(&m_handle);
    }

    /// @brief Returns a constant reference to the object managed by the handle.
    /// @tparam U Type to cast the handle to (e.g., VkBuffer, VkImage, VkDeviceMemory, GLFWwindow)
    /// @return A constant reference to the managed object.
    template <typename U = T>
    const U& get() const noexcept {
        return *reinterpret_cast<const U*>(&m_handle);
    }

private:
    T m_handle;
};

} // namespace R3
