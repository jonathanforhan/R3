#pragma once

#include "api/Log.hpp"
#include <cstdlib>

/**
 * @def ASSERT(_X)
 * @brief Asserts a given values is true, if not exit
 * @param _X Expression
 * @note This only applies to non-distribution builds
 */
#if not R3_BUILD_DISTRIBUTION
#define ASSERT(_X)                               \
    do {                                         \
        if (!(_X)) {                             \
            LOG(Error, "assert failed {}", #_X); \
            std::exit(EXIT_FAILURE);             \
        }                                        \
    } while (false)
#else
#define ASSERT(_X) void(0)
#endif

/**
 * @def ENSURE(_X)
 * @brief Ensures a given values is true, if not exit
 * @param _X Expression
 * @note This applies to all builds
 */
#define ENSURE(_X)                               \
    do {                                         \
        if (!(_X)) {                             \
            LOG(Error, "ensure failed {}", #_X); \
            std::exit(EXIT_FAILURE);             \
        }                                        \
    } while (false)
