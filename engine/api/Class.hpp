#pragma once

#include "engine/api/Api.hpp"

#define R3_CTOR_DEFAULT(_Class) _Class() = default
#define R3_DTOR_DEFAULT(_Class) ~_Class() noexcept = default
#define R3_MOVE_DEFAULT(_Class)                     \
    _Class(_Class&&) noexcept            = default; \
    _Class& operator=(_Class&&) noexcept = default
#define R3_COPY_DEFAULT(_Class)                 \
    _Class(const _Class&)            = default; \
    _Class& operator=(const _Class&) = default
#define R3_CTOR_DELETE(_Class) _Class() = delete
#define R3_COPY_DELETE(_Class)                 \
    _Class(const _Class&)            = delete; \
    _Class& operator=(const _Class&) = delete
#define R3_MOVE_DELETE(_Class)                     \
    _Class(_Class&&) noexcept            = delete; \
    _Class& operator=(_Class&&) noexcept = delete
