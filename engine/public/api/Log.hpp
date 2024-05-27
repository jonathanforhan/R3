/*******************************************************************************
 * @file Log.hpp
 * @brief Provides logging API using fmt library
 ******************************************************************************/

#pragma once

#include <cstdio>
#include <fmt/core.h>

/** @cond TURN_OFF_DOXYGEN */
#if R3_LOG_VERBOSE
#define R3_LOG_Verbose(_Fmt, ...) fmt::print(stderr, fmt::runtime(_Fmt), ##__VA_ARGS__)
#else
#define R3_LOG_Verbose(_Fmt, ...) void(0)
#endif
#if R3_LOG_INFO
#define R3_LOG_Info(_Fmt, ...) fmt::print(stderr, fmt::runtime(_Fmt), ##__VA_ARGS__)
#else
#define R3_LOG_Info(_Fmt, ...) void(0)
#endif
#if R3_LOG_WARNING
#define R3_LOG_Warning(_Fmt, ...) fmt::print(stderr, fmt::runtime(_Fmt), ##__VA_ARGS__)
#else
#define R3_LOG_Warning(_Fmt, ...) void(0)
#endif
#if R3_LOG_ERROR
#define R3_LOG_Error(_Fmt, ...) fmt::print(stderr, fmt::runtime(_Fmt), ##__VA_ARGS__)
#else
#define R3_LOG_Error(_Fmt, ...) void(0)
#endif

#define __detail__R3_STRINGIZE(_X) #_X

#define __detail__R3_LOG(_Level, _Line, _Fmt, ...) \
    R3_LOG_##_Level("R3 " #_Level ":" __FUNCTION__ ":" __detail__R3_STRINGIZE(_Line) "\n" _Fmt, ##__VA_ARGS__);
/** @endcond */

/*******************************************************************************
 * @def LOG(_Level, ...)
 * @brief Logs a message to stderr only if R3_LOG_{Level} is setup in build
 *
 * Every message gets a newline appended
 *
 * @code
 * LOG(Info, "Hello, {}!", "world");
 *
 * -- output --
 * R3 Info:{__FUNCTION__}:{__LINE__}
 * Hello, world!
 * @endcode
 *
 * @param _Level Log level [Verbose|Info|Warning|Error]
 * @param ... Variadic arguements to fmt string
 ******************************************************************************/
#define LOG(_Level, ...) __detail__R3_LOG(_Level, __LINE__, ##__VA_ARGS__)
