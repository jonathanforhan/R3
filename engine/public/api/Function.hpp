#pragma once

#include <tuple>

// https://stackoverflow.com/questions/7943525/is-it-possible-to-figure-out-the-parameter-type-and-return-type-of-a-lambda

namespace R3 {

template <typename T>
struct FunctionTraits : public FunctionTraits<decltype(&T::operator())> {};
// For generic types, directly use the result of the signature of its 'operator()'

template <typename ClassType, typename ReturnType, typename... Args>
struct FunctionTraits<ReturnType (ClassType::*)(Args...)>
// we specialize for pointers to member function
{
    using Arity = std::integral_constant<std::size_t, sizeof...(Args)>;
    template <std::size_t i>
    using ArgType = typename std::tuple_element<i, std::tuple<Args...>>::type;
    using ResultType = ReturnType;
};

template <typename ClassType, typename ReturnType, typename... Args>
struct FunctionTraits<ReturnType (ClassType::*)(Args...) const>
// we specialize for pointers to member function
{
    using Arity = std::integral_constant<std::size_t, sizeof...(Args)>;
    template <std::size_t i>
    using ArgType = typename std::tuple_element<i, std::tuple<Args...>>::type;
    using ResultType = ReturnType;
};

template <typename ClassType, typename ReturnType, typename... Args>
struct FunctionTraits<ReturnType (ClassType::*)(Args..., ...)>
// we specialize for pointers to member function
{
    using Arity = std::integral_constant<std::size_t, sizeof...(Args)>;
    template <std::size_t i>
    using ArgType = typename std::tuple_element<i, std::tuple<Args...>>::type;
    using ResultType = ReturnType;
};

template <typename ClassType, typename ReturnType, typename... Args>
struct FunctionTraits<ReturnType (ClassType::*)(Args..., ...) const>
// we specialize for pointers to member function
{
    using Arity = std::integral_constant<std::size_t, sizeof...(Args)>;
    template <std::size_t i>
    using ArgType = typename std::tuple_element<i, std::tuple<Args...>>::type;
    using ResultType = ReturnType;
};

} // namespace R3