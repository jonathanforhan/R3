/**
 * @file Hash.hpp
 * @brief Compile-time string hashing utilities
 *
 * Contains compile time hashing functions for efficent string-to-hash lookups
 */

#pragma once

#include <string_view>
#include "Types.hpp"

namespace R3::hash {

/**
 * @brief Computes FNV-1a compile time hash
 *
 * @param str String view to hash
 * @return FNV-1a hash value
 *
 * @code
 * constexpr uint64 hash = R3::hash::fnv1a("my_string");
 * @endcode
 */
constexpr uint64 fnv1a(std::string_view str) noexcept {
    uint64 hash = 14695981039346656037ULL; // FNV offset basis
    for (char c : str) {
        hash ^= static_cast<uint64>(c);
        hash *= 1099511628211ULL; // FNV prime
    }
    return hash;
}

/**
 * @brief Computes DJB2 compile time hash
 *
 * @param str String view to hash
 * @return DJB2 hash value
 *
 * @code
 * constexpr uint64 hash = R3::hash::djb2("my_string");
 * @endcode
 */
constexpr uint64 djb2(std::string_view str) noexcept {
    uint64 hash = 5381;
    for (char c : str) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

struct Param {
    /**
     * @brief Param constructor is NOT explicit making it useful as a parameter when costeval hashing is needed.
     * @param str String view to hash
     */
    constexpr Param(std::string_view str) noexcept
        : id{fnv1a(str)} {}

    template <usize N>
    constexpr Param(const char (&str)[N]) noexcept
        : id{fnv1a(std::string_view{str, N - 1})} {}

    constexpr operator uint64() const { return id; }

    const uint64 id;
};

} // namespace R3::hash