#pragma once

#if R3_BUILD_DIST
#error "Todo.hpp included in a dist build"
#endif

#include <sstream>
#include <stdexcept>
#include <string>

/// @def TODO(_Msg)
/// @brief TODO will throw a compile error if built in dist configuration, else it will throw a runtime error if hit

namespace R3::detail::todo {
template <typename... Args>
/// @private
inline std::string _fmt(Args&&... args) {
    std::stringstream ss;
    ((ss << args), ...);
    return ss.str();
}
} // namespace R3::detail::todo

#define TODO(_Msg)                                                                                    \
    do {                                                                                              \
        auto s = R3::detail::todo::_fmt("R3 todo: " _Msg ", File: ", __FILE__, ", Line: ", __LINE__); \
        throw std::runtime_error(s);                                                                  \
    } while (0)
