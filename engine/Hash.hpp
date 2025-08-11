#pragma once

#include <string_view>
#include "Types.hpp"

namespace R3::hash {

consteval uint64 fnv1a(std::string_view str) {
    uint64 hash = 14695981039346656037ULL; // FNV offset basis
    for (char c : str) {
        hash ^= static_cast<uint64>(c);
        hash *= 1099511628211ULL; // FNV prime
    }
    return hash;
}

consteval uint64 djb2(std::string_view str) {
    uint64 hash = 5381;
    for (char c : str) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

} // namespace R3::hash