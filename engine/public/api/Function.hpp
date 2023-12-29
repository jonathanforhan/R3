#pragma once

/// @file Function.hpp
/// @brief Provides a means to deduce the type of lambdas passed into a templated function
/// used in various applications like the Event system
/// https://stackoverflow.com/questions/7943525/is-it-possible-to-figure-out-the-parameter-type-and-return-type-of-a-lambda

#include <R3>
#include <tuple>

namespace R3 {

/// @cond Doxygen_Supress

/// @brief For generic types, directly use the result of the signature of its 'operator()'
template <typename T>
struct R3_API FunctionTraits : public FunctionTraits<decltype(&T::operator())> {};

/// @brief Deduce FunctionTraits for given Functor
/// @tparam ClassType Function type
/// @tparam ReturnType Function Return Type
/// @tparam ...Args Variadic Arguements
template <typename ClassType, typename ReturnType, typename... Args>
struct R3_API FunctionTraits<ReturnType (ClassType::*)(Args...)> {
    /// @brief Number of Arguments
    using Arity = std::integral_constant<std::size_t, sizeof...(Args)>;

    /// @brief Argument type gotten as tuple element
    /// @tparam i ith element
    template <std::size_t i>
    using ArgType = typename std::tuple_element_t<i, std::tuple<Args...>>;

    /// @brief Result type alias
    using ResultType = ReturnType;
};

/// @brief Deduce FunctionTraits for given Functor
/// @tparam ClassType Function type
/// @tparam ReturnType Function Return Type
/// @tparam ...Args Variadic Arguements
template <typename ClassType, typename ReturnType, typename... Args>
struct R3_API FunctionTraits<ReturnType (ClassType::*)(Args...) const> {
    /// @brief Number of Arguments
    using Arity = std::integral_constant<std::size_t, sizeof...(Args)>;

    /// @brief Argument type gotten as tuple element
    /// @tparam i ith element
    template <std::size_t i>
    using ArgType = typename std::tuple_element_t<i, std::tuple<Args...>>;

    /// @brief Result type alias
    using ResultType = ReturnType;
};

/// @brief Deduce FunctionTraits for given Functor
/// @tparam ClassType Function type
/// @tparam ReturnType Function Return Type
/// @tparam ...Args Variadic Arguements
template <typename ClassType, typename ReturnType, typename... Args>
struct R3_API FunctionTraits<ReturnType (ClassType::*)(Args..., ...)> {
    /// @brief Number of Arguments
    using Arity = std::integral_constant<std::size_t, sizeof...(Args)>;

    /// @brief Argument type gotten as tuple element
    /// @tparam i ith element
    template <std::size_t i>
    using ArgType = typename std::tuple_element_t<i, std::tuple<Args...>>;

    /// @brief Result type alias
    using ResultType = ReturnType;
};

/// @brief Deduce FunctionTraits for given Functor
/// @tparam ClassType Function type
/// @tparam ReturnType Function Return Type
/// @tparam ...Args Variadic Arguements
template <typename ClassType, typename ReturnType, typename... Args>
struct R3_API FunctionTraits<ReturnType (ClassType::*)(Args..., ...) const> {
    /// @brief Number of Arguments
    using Arity = std::integral_constant<std::size_t, sizeof...(Args)>;

    /// @brief Argument type gotten as tuple element
    /// @tparam i ith element
    template <std::size_t i>
    using ArgType = typename std::tuple_element_t<i, std::tuple<Args...>>;

    /// @brief Result type alias
    using ResultType = ReturnType;
};

/// @endcond Doxygen_Supress

} // namespace R3