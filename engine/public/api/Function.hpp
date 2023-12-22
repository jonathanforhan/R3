#pragma once

// https://stackoverflow.com/questions/7943525/is-it-possible-to-figure-out-the-parameter-type-and-return-type-of-a-lambda

#include <tuple>

namespace R3 {

// For generic types, directly use the result of the signature of its 'operator()'
template <typename T>
struct FunctionTraits : public FunctionTraits<decltype(&T::operator())> {};

// we specialize for pointers to member function
template <typename ClassType, typename ReturnType, typename... Args>
struct FunctionTraits<ReturnType (ClassType::*)(Args...)> {
    using Arity = std::integral_constant<std::size_t, sizeof...(Args)>;

    template <std::size_t i>
    using ArgType = typename std::tuple_element_t<i, std::tuple<Args...>>;

    using ResultType = ReturnType;
};

template <typename ClassType, typename ReturnType, typename... Args>
struct FunctionTraits<ReturnType (ClassType::*)(Args...) const> {
    using Arity = std::integral_constant<std::size_t, sizeof...(Args)>;

    template <std::size_t i>
    using ArgType = typename std::tuple_element_t<i, std::tuple<Args...>>;

    using ResultType = ReturnType;
};

template <typename ClassType, typename ReturnType, typename... Args>
struct FunctionTraits<ReturnType (ClassType::*)(Args..., ...)> {
    using Arity = std::integral_constant<std::size_t, sizeof...(Args)>;

    template <std::size_t i>
    using ArgType = typename std::tuple_element_t<i, std::tuple<Args...>>;

    using ResultType = ReturnType;
};

template <typename ClassType, typename ReturnType, typename... Args>
struct FunctionTraits<ReturnType (ClassType::*)(Args..., ...) const> {
    using Arity = std::integral_constant<std::size_t, sizeof...(Args)>;

    template <std::size_t i>
    using ArgType = typename std::tuple_element_t<i, std::tuple<Args...>>;

    using ResultType = ReturnType;
};

} // namespace R3