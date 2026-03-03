/// @file TypeTraits.hpp
/// @brief Type traits utilities

#pragma once

#include <concepts>

namespace R3 {

/// @brief Compile-time check if type T is the same as any of the types in Ts
/// @tparam T     The type to check
/// @tparam ...Ts The list of types to compare against
template <typename T, typename... Ts>
constexpr bool is_one_of_v = (std::same_as<T, Ts> || ...);

} // namespace R3
