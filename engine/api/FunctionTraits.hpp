/// @file FunctionTraits.hpp
/// @brief Function traits extraction utilities
///
/// `Arity`:      A compile-time constant representing the number of arguments the function takes.
/// `ArgType<i>`: A type alias to get the type of the i-th argument type of the function.
/// `ResultType`: The return type of the function.

#pragma once

#include <tuple>
#include <type_traits>
#include "engine/api/Api.hpp"

namespace R3 {

/// @brief Extracts function traits from a callable type.
/// @tparam T The callable type from which to extract function traits.
template <typename T>
struct R3_API FunctionTraits : public FunctionTraits<decltype(&T::operator())> {};

//// @brief Specialization for non-const noexcept(false) Functions.
/// @tparam ReturnType Function return type.
/// @tparam ClassType  Lambda class R3_API type.
/// @tparam ...Args    Lambda arguments.
template <typename ReturnType, typename ClassType, typename... Args>
struct R3_API FunctionTraits<ReturnType (ClassType::*)(Args...)> {
    using Arity = std::integral_constant<std::size_t, sizeof...(Args)>;
    template <std::size_t i>
    using ArgType    = typename std::tuple_element_t<i, std::tuple<Args...>>;
    using ResultType = ReturnType;
};

//// @brief Specialization for const noexcept(false) Functions.
/// @tparam ReturnType Function return type.
/// @tparam ClassType  Lambda class R3_API type.
/// @tparam ...Args    Lambda arguments.
template <typename ReturnType, typename ClassType, typename... Args>
struct R3_API FunctionTraits<ReturnType (ClassType::*)(Args...) const> {
    using Arity = std::integral_constant<std::size_t, sizeof...(Args)>;
    template <std::size_t i>
    using ArgType    = typename std::tuple_element_t<i, std::tuple<Args...>>;
    using ResultType = ReturnType;
};

//// @brief Specialization for non-const noexcept(true) Functions.
/// @tparam ReturnType Function return type.
/// @tparam ClassType  Lambda class R3_API type.
/// @tparam ...Args    Lambda arguments.
template <typename ReturnType, typename ClassType, typename... Args>
struct R3_API FunctionTraits<ReturnType (ClassType::*)(Args...) noexcept> {
    using Arity = std::integral_constant<std::size_t, sizeof...(Args)>;
    template <std::size_t i>
    using ArgType    = typename std::tuple_element_t<i, std::tuple<Args...>>;
    using ResultType = ReturnType;
};

//// @brief Specialization for const noexcept(true) Functions.
/// @tparam ReturnType Function return type.
/// @tparam ClassType  Lambda class R3_API type.
/// @tparam ...Args    Lambda arguments.
template <typename ReturnType, typename ClassType, typename... Args>
struct R3_API FunctionTraits<ReturnType (ClassType::*)(Args...) const noexcept> {
    using Arity = std::integral_constant<std::size_t, sizeof...(Args)>;
    template <std::size_t i>
    using ArgType    = typename std::tuple_element_t<i, std::tuple<Args...>>;
    using ResultType = ReturnType;
};

} // namespace R3