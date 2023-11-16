#pragma once
#if R3_BUILD_DIST
#error "Todo.hpp included in a dist build"
#endif

#include <sstream>
#include <stdexcept>
#include <string>

namespace R3::detail::todo {
template <typename... Args>
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
