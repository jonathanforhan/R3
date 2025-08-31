#pragma once

#if _WIN32
#define R3_API    __declspec(dllexport)
#define R3_EXPORT __declspec(dllexport)
#define R3_IMPORT __declspec(dllimport)
#define R3_MODULE extern "C" __declspec(dllexport)
#else
#define R3_API
#define R3_EXPORT
#define R3_IMPORT
#define R3_MODULE extern "C"
#endif