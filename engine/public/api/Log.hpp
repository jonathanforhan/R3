#pragma once
#include <iostream>

/// @def LOG(_Level, ...)
/// @brief Log message, LogLevel dependant
/// All arguements are seperated by spaces
/// @param _Level log level [Verbose|Info|Warning|Error]
/// @param ... variadic log arguements

#if R3_LOGGING_ENABLED

namespace R3::detail::logging {

/// @private
inline void log() {
    std::cout << '\n';
}

/// @private
template <typename T, typename... Args>
inline void log(T x, Args&&... args) {
    std::cout << x << ' ';
    ::R3::detail::logging::log(args...);
}

} // namespace R3::detail::logging

#if R3_LOG_VERBOSE
#define R3_LOG_Verbose(...) ::R3::detail::logging::log(__VA_ARGS__)
#else
#define R3_LOG_Verbose(...) void(0)
#endif
#if R3_LOG_INFO
#define R3_LOG_Info(...) ::R3::detail::logging::log(__VA_ARGS__)
#else
#define R3_LOG_Info(...) void(0)
#endif
#if R3_LOG_WARNING
#define R3_LOG_Warning(...) ::R3::detail::logging::log(__VA_ARGS__)
#else
#define R3_LOG_Warning(...) void(0)
#endif
#if R3_LOG_ERROR
#define R3_LOG_Error(...) ::R3::detail::logging::log(__VA_ARGS__)
#else
#define R3_LOG_Error(...) void(0)
#endif

#define LOG(_Level, ...)                                 \
    do {                                                 \
        R3_LOG_##_Level("R3 " #_Level ":", __VA_ARGS__); \
    } while (0)
#else
#define LOG(_Level, ...) void(0)
#endif
