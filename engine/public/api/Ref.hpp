#pragma once
#include "api/Check.hpp"
#include "api/NoExcept.hpp"

namespace R3 {

/// @brief Reference class
/// Ref's internal pointer is set null on creation however, you CANNOT assign nullptr to it after initial creation,
/// this means that it can default initialed however still has guarentees that anytime it's assigned, copied or referrenced
/// it is NOT null. Anytime the Ref is std::move'd it is set to null and unusable unless reassigned to
/// Ref guarentees that the pointer is not null, it DOES NOT do anything like freeing the pointer, in fact never use
/// Ref for things that you own, it is strictly for referencing unowned objects
/// @tparam T underlying type
template <typename T>
class Ref {
public:
    Ref() R3_NOEXCEPT
        : m_ptr(nullptr) {}

    Ref(T* ptr) R3_NOEXCEPT
        : m_ptr(ptr) {
        CHECK(m_ptr != nullptr);
    }

    Ref(const Ref<T>& src) R3_NOEXCEPT {
        CHECK(src.m_ptr != nullptr);
        m_ptr = src.m_ptr;
    }

    Ref& operator=(const Ref<T>& src) R3_NOEXCEPT {
        CHECK(src.m_ptr != nullptr);
        m_ptr = src.m_ptr;
        return *this;
    }

    Ref(Ref<T>&& src) R3_NOEXCEPT {
        CHECK(src.m_ptr != nullptr);
        m_ptr = src.m_ptr;
        src.m_ptr = nullptr;
    }

    Ref& operator=(Ref<T>&& src) R3_NOEXCEPT {
        CHECK(src.m_ptr != nullptr);
        m_ptr = src.m_ptr;
        src.m_ptr = nullptr;
        return *this;
    }

    Ref& operator=(T* ptr) R3_NOEXCEPT {
        CHECK(ptr != nullptr);
        m_ptr = ptr;
        return *this;
    }

    operator T*() R3_NOEXCEPT {
        CHECK(m_ptr != nullptr);
        return m_ptr;
    }

    operator T*() const R3_NOEXCEPT {
        CHECK(m_ptr != nullptr);
        return m_ptr;
    }

    constexpr auto& operator*() R3_NOEXCEPT {
        CHECK(m_ptr != nullptr);
        return *m_ptr;
    }

    constexpr auto& operator*() const R3_NOEXCEPT {
        CHECK(m_ptr != nullptr);
        return *m_ptr;
    }

    constexpr T* operator->() R3_NOEXCEPT {
        CHECK(m_ptr != nullptr);
        return m_ptr;
    }

    constexpr T* operator->() const R3_NOEXCEPT {
        CHECK(m_ptr != nullptr);
        return m_ptr;
    }

    constexpr auto operator<=>(const Ref<T>&) const = default;

    constexpr bool operator==(void* vp) const {
        return m_ptr == vp;
    }

    constexpr T* operator+(int i) const R3_NOEXCEPT { return m_ptr + i; }
    constexpr T* operator-(int i) const R3_NOEXCEPT { return m_ptr - i; }
    constexpr void operator+=(int i) R3_NOEXCEPT { return m_ptr += i; }
    constexpr void operator-=(int i) R3_NOEXCEPT { return m_ptr -= i; }
    constexpr auto& operator++() R3_NOEXCEPT {
        m_ptr++;
        return m_ptr;
    }
    constexpr auto operator++(int) R3_NOEXCEPT {
        auto tmp = m_ptr;
        (*this)++;
        return tmp;
    }
    constexpr auto& operator--() R3_NOEXCEPT {
        m_ptr--;
        return m_ptr;
    }
    constexpr auto operator--(int) R3_NOEXCEPT {
        auto tmp = m_ptr;
        (*this)--;
        return tmp;
    }

    constexpr T* get() {
        return m_ptr;
    }

    constexpr T* get() const {
        return m_ptr;
    }

private:
    T* m_ptr;
};

} // namespace R3