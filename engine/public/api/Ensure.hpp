#pragma once

/// @file Ensure.hpp
/// @brief Provides a means to ensure that a given expression evaluates to true, applied to all builds

#include <sstream>
#include <stdexcept>

/// @def ENSURE(_Expr)
/// @brief ENSURE throws an exception if the expression is evaluated as false
/// @param _Expr expression to evaluate
/// @note runs in any project configurations

#define ENSURE(_Expr)                                                                            \
    do {                                                                                         \
        if (!(_Expr))                                                                            \
            throw std::runtime_error((std::stringstream("R3 ensure failure: '")                  \
                                      << #_Expr << "' " << __FUNCTION__ << " File: " << __FILE__ \
                                      << " Line: " << __LINE__)                                  \
                                         .str());                                                \
    } while (0)
