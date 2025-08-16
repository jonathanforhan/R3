#pragma once

#include <tuple>
#include <type_traits>

namespace R3 {

// Provides a means to deduce the type of lambdas passed into a templated function
// https://stackoverflow.com/questions/7943525/is-it-possible-to-figure-out-the-parameter-type-and-return-type-of-a-lambda

template <typename T>
struct FunctionTraits : public FunctionTraits<decltype(&T::operator())> {};

template <typename ReturnType, typename ClassType, typename... Args>
struct FunctionTraits<ReturnType (ClassType::*)(Args...)> {
    using Arity = std::integral_constant<std::size_t, sizeof...(Args)>;
    template <std::size_t i>
    using ArgType    = typename std::tuple_element_t<i, std::tuple<Args...>>;
    using ArgPack    = std::tuple<Args...>;
    using ResultType = ReturnType;
};

template <typename ReturnType, typename ClassType, typename... Args>
struct FunctionTraits<ReturnType (ClassType::*)(Args...) const> {
    using Arity = std::integral_constant<std::size_t, sizeof...(Args)>;
    template <std::size_t i>
    using ArgType    = typename std::tuple_element_t<i, std::tuple<Args...>>;
    using ArgPack    = std::tuple<Args...>;
    using ResultType = ReturnType;
};

} // namespace R3