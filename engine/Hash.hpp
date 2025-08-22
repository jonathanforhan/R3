/// @file Hash.hpp
/// @brief Compile-time string hashing utilities
///
/// Contains compile time hashing functions for efficent string-to-hash lookups

#pragma once

#include <string_view>
#include "Types.hpp"

namespace R3::hash {

/// @brief Computes FNV-1a compile time hash
///
/// @param str string view to hash
/// @return FNV-1a hash value
///
/// @code
/// constexpr uint64 hash = R3::hash::fnv1a("my_string");
/// @endcode
constexpr uint64 fnv1a(std::string_view str) noexcept {
    uint64 hash = 14695981039346656037ULL; // FNV offset basis
    for (char c : str) {
        hash ^= static_cast<uint64>(c);
        hash *= 1099511628211ULL; // FNV prime
    }
    return hash;
}

/// @brief Computes DJB2 compile time hash
///
/// @param str string view to hash
/// @return DJB2 hash value
///
/// @code
/// constexpr uint64 hash = R3::hash::djb2("my_string");
/// @endcode
constexpr uint64 djb2(std::string_view str) noexcept {
    uint64 hash = 5381;
    for (char c : str) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

/// @brief Container for 64-bit uint uuid
class uuid {
public:
    /// @brief uuid constructor is NOT explicit making it useful as a parameter when costexpr hashing is needed.
    /// @param str string view to hash
    constexpr uuid(std::string_view str) noexcept
        : m_id{fnv1a(str)} {}

    /// @brief uuid constructor is NOT explicit making it useful as a parameter when costexpr hashing is needed.
    /// @tparam N  implicit
    /// @param str static cstring to hash
    template <usize N>
    constexpr uuid(const char (&str)[N]) noexcept
        : m_id{fnv1a(std::string_view{str, N - 1})} {}

    /// @brief Implicit conversion to uint64
    constexpr operator uint64() const noexcept { return m_id; }

    /// @brief Checks if this uuid is equal to another uuid.
    /// @param other The uuid to compare with this uuid.
    /// @return true if the two uuids are equal; otherwise, false.
    constexpr bool operator==(uuid other) const noexcept { return m_id == other.m_id; }

    /// @brief Checks whether this uuid is not equal to another uuid.
    /// @param other The uuid to compare with this uuid.
    /// @return true if the two uuids are not equal; otherwise, false.
    constexpr bool operator!=(uuid other) const noexcept { return m_id != other.m_id; }

private:
    const uint64 m_id;
};

} // namespace R3::hash