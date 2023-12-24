#pragma once

/// @file Construct.hpp
/// @brief Provides macros for more readable and less error-prone Rule-5 implementation

/// @def DEFAULT_CONSTRUCT(_Class)
#define DEFAULT_CONSTRUCT(_Class) _Class() = default

/// @def NO_CONSTRUCT(_Class)
#define NO_CONSTRUCT(_Class) _Class() = delete

/// @def DEFAULT_COPY(_Class)
#define DEFAULT_COPY(_Class)         \
    _Class(const _Class&) = default; \
    _Class& operator=(const _Class&) = default

/// @def NO_COPY(_Class)
#define NO_COPY(_Class)             \
    _Class(const _Class&) = delete; \
    _Class& operator=(const _Class&) = delete

/// @def DEFAULT_MOVE(_Class)
#define DEFAULT_MOVE(_Class)             \
    _Class(_Class&&) noexcept = default; \
    _Class& operator=(_Class&&) noexcept = default

/// @def NO_MOVE(_Class)
#define NO_MOVE(_Class)                 \
    _Class(_Class&&) noexcept = delete; \
    _Class& operator=(_Class&&) noexcept = delete
