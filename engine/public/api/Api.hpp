#pragma once

// clang-format off
#if _WIN32
#    define R3_API __declspec(dllexport)
#    if R3_ENGINE
#        define R3_EXPORT __declspec(dllexport)
#    else
#        define R3_EXPORT __declspec(dllimport)
#        define R3_DLL __declspec(dllexport)
#    endif
#else
#    define R3_API
#    if R3_ENGINE
#        define R3_EXPORT
#    else
#        define R3_EXPORT
#        define R3_DLL
#    endif
#endif
// clang-format on
