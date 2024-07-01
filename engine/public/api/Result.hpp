#pragma once

#include <expected>

/// @def R3_PROPAGATE
/// @brief Propgate error by returning it from function.
/// For this to work the function must return a Result type.
#define R3_PROPAGATE(_X) \
    if (!_X) return std::unexpected(_X.error())

namespace R3 {

/// @brief Error describing some failed or invalid state.
enum class Error {
    Timeout,
    NotReady,
    InvalidArgument,
    InvalidFilepath,
    UnsupportedFeature,
    AllocationFailure,
    InitializationFailure,
    UnknownFailure,
};

/// @brief Result type for returning possible R3::Errors.
/// @tparam T Type expected
template <typename T>
using Result = std::expected<T, Error>;

} // namespace R3