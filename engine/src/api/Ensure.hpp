#pragma once
#include <sstream>
#include <stdexcept>
#include <string>

namespace R3::detail::ensure {
template <typename... Args>
inline std::string _fmt(Args&&... args) {
    std::stringstream ss;
    ((ss << args), ...);
    return ss.str();
}
} // namespace R3::detail::ensure

#define ENSURE(_Expr)                                                                                                  \
    do {                                                                                                               \
        if (!(_Expr)) {                                                                                                \
            auto s =                                                                                                   \
                R3::detail::ensure::_fmt("R3 ensure failure: '", #_Expr, "', File: ", __FILE__, ", Line: ", __LINE__); \
            throw std::runtime_error(s);                                                                               \
        }                                                                                                              \
    } while (0)
#define ENSUREF(_Expr, ...)                                                                          \
    do {                                                                                             \
        if (!(_Expr)) {                                                                              \
            auto s = R3::detail::ensure::_fmt("R3 ensure failure: '", #_Expr, "', File: ", __FILE__, \
                                              ", Line: ", __LINE__, ", Message: ", __VA_ARGS__);     \
            throw std::runtime_error(s);                                                             \
        }                                                                                            \
    } while (0)
