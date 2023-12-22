#pragma once

#include <format>
#include <stdexcept>

/// @def ENSURE(_Expr)
/// @brief ENSURE throws an exception if the expression is evaluated as false
/// @param _Expr expression to evaluate
/// @note runs in any project configurations

#define ENSURE(_Expr)                                                                                       \
    do {                                                                                                    \
        if (!(_Expr)) {                                                                                     \
            throw std::runtime_error(std::format(                                                           \
                "R3 ensure failure: '{}' {} File: {} Line: {}", #_Expr, __FUNCTION__, __FILE__, __LINE__)); \
        }                                                                                                   \
    } while (0)
