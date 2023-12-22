#pragma once

#include <format>
#include <stdexcept>

/// @def CHECK(_Expr)
/// @brief CHECK throws an exception if the expression is evaluated as false
/// @param _Expr expression to evaluate
/// @warning CHECK only applies in debug and release builds, not dist builds, see ENSURE.

#if not R3_BUILD_DIST
#define CHECK(_Expr)                                                                                                   \
    do {                                                                                                               \
        if (!(_Expr)) {                                                                                                \
            throw std::runtime_error(                                                                                  \
                std::format("R3 check failure: '{}' {} File: {} Line: {}", #_Expr, __FUNCTION__, __FILE__, __LINE__)); \
        }                                                                                                              \
    } while (0)
#else
#define CHECK(_Expr) void(0)
#endif
