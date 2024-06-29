/**
 * @file Log.hpp
 * @copyright GNU Public License
 *
 * Provides logging API using fmt library.
 */

#pragma once

#include <fmt/core.h>

/** @cond TURN_OFF_DOXYGEN */
#if R3_LOG_VERBOSE
#define __detail__R3_LOG_Verbose(_Fmt, ...) fmt::println(stderr, fmt::runtime(_Fmt), ##__VA_ARGS__)
#else
#define __detail__R3_LOG_Verbose(_Fmt, ...) (void)0
#endif
#if R3_LOG_INFO
#define __detail__R3_LOG_Info(_Fmt, ...) fmt::println(stderr, fmt::runtime(_Fmt), ##__VA_ARGS__)
#else
#define __detail__R3_LOG_Info(_Fmt, ...) (void)0
#endif
#if R3_LOG_WARNING
#define __detail__R3_LOG_Warning(_Fmt, ...) fmt::println(stderr, fmt::runtime(_Fmt), ##__VA_ARGS__)
#else
#define __detail__R3_LOG_Warning(_Fmt, ...) (void)0
#endif
#if R3_LOG_ERROR
#define __detail__R3_LOG_Error(_Fmt, ...) fmt::println(stderr, fmt::runtime(_Fmt), ##__VA_ARGS__)
#else
#define __detail__R3_LOG_Error(_Fmt, ...) (void)0
#endif

#define __detail__R3_STRINGIZE(_X) #_X

#define __detail__R3_LOG(_Level, _Line, _Fmt, ...) \
    __detail__R3_LOG_##_Level("R3 " #_Level ":" __FUNCTION__ ":" __detail__R3_STRINGIZE(_Line) "\n" _Fmt, ##__VA_ARGS__)
/** @endcond TURN_OFF_DOXYGEN */

/**
 * @def R3_LOG(_Level, ...)
 * Logs a message to stderr only if R3_LOG_{Level} is enabled in build.
 * Uses fmt::println. Level options are [Verbose|Info|Warning|Error]
 * @code
 * R3_LOG(Info, "Hello, {}!", "world");
 * // output
 * // R3 Info:{__FUNCTION__}:{__LINE__} Hello, world!
 * @endcode
 * @param _Level Log level [Verbose|Info|Warning|Error]
 * @param ... Variadic arguements to fmt string
 */
#define R3_LOG(_Level, ...) __detail__R3_LOG(_Level, __LINE__, ##__VA_ARGS__)
