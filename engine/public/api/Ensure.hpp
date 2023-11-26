#pragma once
#include <sstream>
#include <stdexcept>
#include <string>

/// @def ENSURE(_Expr)
/// @brief ENSURE throws an exception if the expression is evaluated as false
/// @param _Expr expression to evaluate
/// @note runs in any project configurations

namespace R3::detail::ensure {
template <typename... Args>
/// @private
inline std::string _fmt(Args&&... args) {
    std::stringstream ss;
    ((ss << args), ...);
    return ss.str();
}
} // namespace R3::detail::ensure

#define ENSURE(_Expr)                                                                          \
    do {                                                                                       \
        if (!(_Expr)) {                                                                        \
            throw std::runtime_error(R3::detail::ensure::_fmt(                                 \
                "R3 ensure failure: '", #_Expr, "', File: ", __FILE__, ", Line: ", __LINE__)); \
        }                                                                                      \
    } while (0)
