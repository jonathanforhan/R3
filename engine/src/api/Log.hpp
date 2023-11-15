#pragma once
#include <iostream>

#if R3_LOGGING_ENABLED
namespace R3::detail::logging {
inline void _LOG() {
    std::cout << std::endl;
}
template <typename T, typename... Args>
inline void _LOG(T x, Args&&... args) {
    std::cout << x << ' ';
    _LOG(args...);
}
} // namespace R3::detail::logging

#if R3_LOG_VERBOSE
#define R3_LOG_Verbose R3::detail::logging::_LOG
#else
#define R3_LOG_Verbose void(0)
#endif
#if R3_LOG_INFO
#define R3_LOG_Info R3::detail::logging::_LOG
#else
#define R3_LOG_Info void(0)
#endif
#if R3_LOG_WARNING
#define R3_LOG_Warning R3::detail::logging::_LOG
#else
#define R3_LOG_Warning void(0)
#endif
#if R3_LOG_ERROR
#define R3_LOG_Error R3::detail::logging::_LOG
#else
#define R3_LOG_Error void(0)
#endif

#define LOG(_Level, ...)                                 \
    do {                                                 \
        R3_LOG_##_Level("R3 " #_Level ":", __VA_ARGS__); \
    } while (0)
#else
#define LOG(_Level, ...) void(0)
#endif
