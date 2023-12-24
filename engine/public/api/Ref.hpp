#pragma once

/// @file Ref.hpp
/// @brief Sometimes you want a reference but you also want to be default constructable, this provides that

#include "api/Check.hpp"

namespace R3 {

/// @brief Reference class, providing default constructable references
/// Ref<T> is set to null when default initialized, when doing any other operation like dereferencing or assigning
/// a runtime-check is performed to ensure that the refernce is valid. This allows greater flexibility of References
/// at the cost checking. Checks are disabled in Distribution builds so Ref<T> operations must be correct before
/// shipping.
/// @note The Ref<T> should almost never own the object, as it is only a reference
/// @tparam T underlying type
template <typename T>
class Ref {
public:
    /// @brief Initialize to nullptr by default
    Ref()
        : m_ptr(nullptr) {}

    /// @brief Construct Ref using pointer to type
    /// @param ptr
    Ref(T* ptr)
        : m_ptr(ptr) {
        CHECK(m_ptr != nullptr);
    }

    /// @brief Construct copy
    /// @param src
    Ref(const Ref<T>& src) {
        CHECK(src.m_ptr != nullptr);
        m_ptr = src.m_ptr;
    }

    /// @brief Assign copy
    /// @param src
    /// @return
    Ref& operator=(const Ref<T>& src) {
        CHECK(src.m_ptr != nullptr);
        m_ptr = src.m_ptr;
        return *this;
    }

    /// @brief Move
    /// @param src
    Ref(Ref<T>&& src) noexcept {
        CHECK(src.m_ptr != nullptr);
        m_ptr = src.m_ptr;
        src.m_ptr = nullptr;
    }

    /// @brief Move
    /// @param src
    /// @return
    Ref& operator=(Ref<T>&& src) noexcept {
        CHECK(src.m_ptr != nullptr);
        m_ptr = src.m_ptr;
        src.m_ptr = nullptr;
        return *this;
    }

    /// @brief Construct from pointer
    /// @param ptr
    /// @return
    Ref& operator=(auto* ptr) {
        CHECK(ptr != nullptr);
        m_ptr = ptr;
        return *this;
    }

    /// @brief Cast to underlying type
    constexpr operator T*() {
        CHECK(m_ptr != nullptr);
        return m_ptr;
    }

    /// @brief Cast to underlying const type
    constexpr operator T*() const {
        CHECK(m_ptr != nullptr);
        return m_ptr;
    }

    /// @brief Dereference
    constexpr auto& operator*() {
        CHECK(m_ptr != nullptr);
        return *m_ptr;
    }

    /// @brief Const dereference
    constexpr auto& operator*() const {
        CHECK(m_ptr != nullptr);
        return *m_ptr;
    }

    /// @brief Asscess pointer
    /// @return
    constexpr T* operator->() {
        CHECK(m_ptr != nullptr);
        return m_ptr;
    }

    /// @brief Asscess pointer
    /// @return
    constexpr T* operator->() const {
        CHECK(m_ptr != nullptr);
        return m_ptr;
    }

    /// @brief Equality
    /// @param p
    /// @return
    constexpr bool operator==(auto p) const { return m_ptr == p; }

    /// @brief Equality
    /// @param p
    /// @return
    constexpr bool operator!=(auto p) const { return m_ptr != p; }

    /// @brief Access pointer
    /// @return
    constexpr T* get() { return m_ptr; }

    /// @brief Access pointer
    /// @return
    constexpr T* get() const { return m_ptr; }

private:
    T* m_ptr;
};

} // namespace R3