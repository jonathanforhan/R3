#pragma once

#define DEFAULT_CONSTRUCT(_Class) _Class() = default

#define NO_CONSTRUCT(_Class) _Class() = delete

#define DEFAULT_COPY(_Class)                    \
    _Class(const _Class&)            = default; \
    _Class& operator=(const _Class&) = default

#define NO_COPY(_Class)                        \
    _Class(const _Class&)            = delete; \
    _Class& operator=(const _Class&) = delete

#define DEFAULT_MOVE(_Class)                        \
    _Class(_Class&&) noexcept            = default; \
    _Class& operator=(_Class&&) noexcept = default

#define NO_MOVE(_Class)                            \
    _Class(_Class&&) noexcept            = delete; \
    _Class& operator=(_Class&&) noexcept = delete