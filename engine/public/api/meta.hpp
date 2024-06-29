////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file meta.hpp
/// @copyright GNU Public License
/// @brief Provides meta-programming utilities modeled from stl.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <type_traits>

namespace meta {

/// @brief Variadic form of logical oring std::is_same.
/// @tparam T Types to check
/// @tparam Args... The arguements to check against
template <typename T, typename... Args>
concept is_one_of = std::disjunction<std::is_same<T, Args>&&...>::type;

/// @brief Variadic form of logical oring std::is_same_v.
/// @tparam T Types to check
/// @tparam Args... The arguements to check against
template <typename T, typename... Args>
concept is_one_of_v = std::disjunction_v<std::is_same<T, Args>&&...>;

/// @brief Variadic form of logical anding std::is_same.
/// @tparam T Types to check
/// @tparam Args... The arguements to check against
template <typename T, typename... Args>
concept are_same = std::conjunction<std::is_same<T, Args>&&...>::type;

/// @brief Variadic form of logical anding std::is_same_v.
/// @tparam T Types to check
/// @tparam Args... The arguements to check against
template <typename T, typename... Args>
concept are_same_v = std::conjunction_v<std::is_same<T, Args>&&...>;

} // namespace meta
