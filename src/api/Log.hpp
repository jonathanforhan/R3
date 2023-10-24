#pragma once
#include <iostream>
#include <type_traits>

#if R3_LOGGING_ENABLED
#ifndef R3_LOG_VERBOSE
#define R3_LOG_VERBOSE 0
#endif
#ifndef R3_LOG_INFO
#define R3_LOG_INFO 0
#endif
#ifndef R3_LOG_WARNING
#define R3_LOG_WARNING 0
#endif
#ifndef R3_LOG_ERROR
#define R3_LOG_ERROR 0
#endif

namespace R3::logging {
enum class Verbose;
enum class Info;
enum class Warning;
enum class Error;
} // namespace R3::logging

#define LOG(_Level, _Msg)                                                      \
  do {                                                                         \
    using namespace ::R3::logging;                                             \
    if constexpr (R3_LOG_VERBOSE && std::is_same<_Level, Verbose>::value) {    \
      std::cout << "R3 Verbose: " << _Msg << std::endl;                        \
    } else if constexpr (R3_LOG_INFO && std::is_same<_Level, Info>::value) {   \
      std::cout << "R3 Info: " << _Msg << std::endl;                           \
    } else if constexpr (R3_LOG_WARNING &&                                     \
                         std::is_same<_Level, Warning>::value) {               \
      std::cout << "R3 Warning: " << _Msg << std::endl;                        \
    } else if constexpr (R3_LOG_ERROR && std::is_same<_Level, Error>::value) { \
      std::cerr << "R3 Error: " << _Msg << std::endl;                          \
    }                                                                          \
  } while (0)

#define LOGF(_Level, ...)                                                      \
  do {                                                                         \
    using namespace ::R3::logging;                                             \
    if constexpr (R3_LOG_VERBOSE && std::is_same<_Level, Verbose>::value) {    \
      fprintf(stdout, "R3 Verbose: ");                                         \
      fprintf(stdout, __VA_ARGS__);                                            \
      fprintf(stdout, "\n");                                                   \
    } else if constexpr (R3_LOG_INFO && std::is_same<_Level, Info>::value) {   \
      fprintf(stdout, "R3 Info: ");                                            \
      fprintf(stdout, __VA_ARGS__);                                            \
      fprintf(stdout, "\n");                                                   \
    } else if constexpr (R3_LOG_WARNING &&                                     \
                         std::is_same<_Level, Warning>::value) {               \
      fprintf(stdout, "R3 Warning: ");                                         \
      fprintf(stdout, __VA_ARGS__);                                            \
      fprintf(stdout, "\n");                                                   \
    } else if constexpr (R3_LOG_ERROR && std::is_same<_Level, Error>::value) { \
      fprintf(stderr, "R3 Error: ");                                           \
      fprintf(stderr, __VA_ARGS__);                                            \
      fprintf(stderr, "\n");                                                   \
    }                                                                          \
  } while (0)

#else // R3_LOGGING_ENABLED
#define LOG(_Level, _Msg) (void)0
#define LOGF(_Level, ...) (void)0
#endif
