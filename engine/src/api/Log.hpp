#pragma once
#include <iostream>

#if R3_LOGGING_ENABLED
namespace R3::detail::logging {
inline void _log() {
    std::cout << std::endl;
}
template <typename T, typename... Args>
inline void _log(T x, Args&&... args) {
    std::cout << x << ' ';
    _log(args...);
}
template <typename... Args>
inline void _logf(const char* fmt, Args&&... args) {
    printf(fmt, args...);
    printf("\n");
}
} // namespace R3::detail::logging

#if R3_LOG_VERBOSE
#define R3_log_Verbose R3::detail::logging::_log
#define R3_logf_Verbose R3::detail::logging::_logf
#else
#define R3_log_Verbose void(0)
#define R3_logf_Verbose void(0)
#endif
#if R3_LOG_INFO
#define R3_log_Info R3::detail::logging::_log
#define R3_logf_Info R3::detail::logging::_logf
#else
#define R3_log_Info void(0)
#define R3_logf_Info void(0)
#endif
#if R3_LOG_WARNING
#define R3_log_Warning R3::detail::logging::_log
#define R3_logf_Warning R3::detail::logging::_logf
#else
#define R3_log_Warning void(0)
#define R3_logf_Warning void(0)
#endif
#if R3_LOG_ERROR
#define R3_log_Error R3::detail::logging::_log
#define R3_logf_Error R3::detail::logging::_logf
#else
#define R3_log_Error void(0)
#define R3_logf_Error void(0)
#endif

#define LOG(_Level, ...)                                 \
    do {                                                 \
        R3_log_##_Level("R3 " #_Level ":", __VA_ARGS__); \
    } while (0)
#define LOGF(_Level, ...)                                 \
    do {                                                  \
        R3_logf_##_Level("R3 " #_Level ": " __VA_ARGS__); \
    } while (0)
#else
#define LOG(_Level, ...) void(0)
#define LOGF(_Level, ...) void(0)
#endif