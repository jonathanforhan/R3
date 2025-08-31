#pragma once

#include <cstdio>
#include <print>
#include "engine/api/Api.hpp"

#define R3_ANSI_RESET   "\033[0m"
#define R3_ANSI_BLACK   "\033[30m"
#define R3_ANSI_RED     "\033[31m"
#define R3_ANSI_GREEN   "\033[32m"
#define R3_ANSI_YELLOW  "\033[33m"
#define R3_ANSI_BLUE    "\033[34m"
#define R3_ANSI_MAGENTA "\033[35m"
#define R3_ANSI_CYAN    "\033[36m"
#define R3_ANSI_WHITE   "\033[37m"

// Bright colors
#define R3_ANSI_BRIGHT_BLACK   "\033[90m"
#define R3_ANSI_BRIGHT_RED     "\033[91m"
#define R3_ANSI_BRIGHT_GREEN   "\033[92m"
#define R3_ANSI_BRIGHT_YELLOW  "\033[93m"
#define R3_ANSI_BRIGHT_BLUE    "\033[94m"
#define R3_ANSI_BRIGHT_MAGENTA "\033[95m"
#define R3_ANSI_BRIGHT_CYAN    "\033[96m"
#define R3_ANSI_BRIGHT_WHITE   "\033[97m"

// Background colors
#define R3_ANSI_BG_BLACK   "\033[40m"
#define R3_ANSI_BG_RED     "\033[41m"
#define R3_ANSI_BG_GREEN   "\033[42m"
#define R3_ANSI_BG_YELLOW  "\033[43m"
#define R3_ANSI_BG_BLUE    "\033[44m"
#define R3_ANSI_BG_MAGENTA "\033[45m"
#define R3_ANSI_BG_CYAN    "\033[46m"
#define R3_ANSI_BG_WHITE   "\033[47m"

// Text styles
#define R3_ANSI_BOLD          "\033[1m"
#define R3_ANSI_DIM           "\033[2m"
#define R3_ANSI_ITALIC        "\033[3m"
#define R3_ANSI_UNDERLINE     "\033[4m"
#define R3_ANSI_BLINK         "\033[5m"
#define R3_ANSI_REVERSE       "\033[7m"
#define R3_ANSI_STRIKETHROUGH "\033[9m"

#ifdef WIN32
namespace R3::detail {
void R3_API enableWindowsConsoleColors() noexcept;
} // namespace R3::detail
#endif

#define LOG_ERROR(_Fmt, ...)   (std::println(stderr, R3_ANSI_RED "[ERROR] " R3_ANSI_RESET _Fmt, ##__VA_ARGS__))
#define LOG_WARNING(_Fmt, ...) (std::println(stderr, R3_ANSI_YELLOW "[WARNING] " R3_ANSI_RESET _Fmt, ##__VA_ARGS__))
#define LOG_INFO(_Fmt, ...)    (std::println(stdout, R3_ANSI_GREEN "[INFO] " R3_ANSI_RESET _Fmt, ##__VA_ARGS__))
#if 0
#define LOG_VERBOSE(_Fmt, ...) (std::println(stdout, R3_ANSI_CYAN "[VERBOSE] " R3_ANSI_RESET _Fmt, ##__VA_ARGS__))
#else
#define LOG_VERBOSE(_Fmt, ...) (void)0
#endif
#define LOG(_Fmt, ...) (std::println(stdout, _Fmt, ##__VA_ARGS__))