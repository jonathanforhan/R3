#pragma once
#include <sstream>
#include <stdexcept>
#include <string>

/// @def CHECK(_Expr)
/// @brief CHECK throws an exception if the expression is evaluated as false
/// @param _Expr expression to evaluate
/// @warning CHECK only applies in debug and release builds, not dist builds, see ENSURE.

namespace R3::detail::check {
template <typename... Args>
/// @private
inline std::string _fmt(Args&&... args) {
    std::stringstream ss;
    ((ss << args), ...);
    return ss.str();
}
} // namespace R3::detail::check

#if not R3_BUILD_DIST
#define CHECK(_Expr)                                                                                                  \
    do {                                                                                                              \
        if (!(_Expr)) {                                                                                               \
            throw std::runtime_error(                                                                                 \
                R3::detail::check::_fmt("R3 check failure: '", #_Expr, "', File: ", __FILE__, ", Line: ", __LINE__)); \
        }                                                                                                             \
    } while (0)
#else
#define CHECK(_Expr) void(0)
#endif
