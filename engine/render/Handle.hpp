#pragma once

#include <type_traits>
#include <utility>

/**
 * @brief Container for render api handles that implements proper move semantics so a parent class need not write a
 * custom move constructor
 * @tparam T
 */
template <typename T>
requires(std::is_pointer_v<T> || std::is_integral_v<T>)
class Handle {
public:
    Handle() = default;

    constexpr Handle(T h)
        : m_handle(h) {}

    ~Handle() noexcept = default;

    Handle(const Handle&)            = delete;
    Handle& operator=(const Handle&) = delete;

    Handle(Handle&& other) noexcept
        : m_handle(std::exchange(other.m_handle, T{})) {}

    Handle& operator=(Handle&& other) noexcept {
        if (this != &other) {
            m_handle = std::exchange(other.m_handle, T{});
        }
        return *this;
    }

    constexpr T& get() { return m_handle; }
    constexpr const T& get() const { return m_handle; }

    constexpr operator T&() { return m_handle; }
    constexpr operator const T&() const { return m_handle; }

    constexpr operator bool() const { return m_handle == T{}; }

private:
    T m_handle{};
};