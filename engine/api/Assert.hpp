#pragma once

#include <cstdlib>
#include "engine/api/Api.hpp"
#include "engine/core/Log.hpp"

#if R3_DEBUG
#define R3_ASSERT(_Exp, ...)                                       \
    do {                                                           \
        if (!(_Exp)) {                                             \
            LOG_ERROR("R3_ASSERT failed: {} " __VA_ARGS__, #_Exp); \
            std::abort();                                          \
        }                                                          \
    } while (false)
#else
#define R3_ASSERT(_Exp, ...) (void)0
#endif