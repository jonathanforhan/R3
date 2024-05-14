#pragma once

/// @file Check.hpp
/// @brief Macro for checking results, only applied to non-distribution builds

#include <sstream>
#include <stdexcept>

/// @def CHECK(_Expr)
/// @brief CHECK throws an exception if the expression is evaluated as false
/// @param _Expr expression to evaluate
/// @warning CHECK only applies in debug and release builds, not dist builds, see ENSURE.

#if not R3_BUILD_DIST
#define CHECK(_Expr)                                                                             \
    do {                                                                                         \
        if (!(_Expr))                                                                            \
            throw std::runtime_error((std::stringstream("R3 check failure: '")                   \
                                      << #_Expr << "' " << __FUNCTION__ << " File: " << __FILE__ \
                                      << " Line: " << __LINE__)                                  \
                                         .str());                                                \
    } while (0)
#else
#define CHECK(_Expr) void(0)
#endif
