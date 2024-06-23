#pragma once

#include <cmath>
#include <concepts>
#include <cstdint>
#include <glm/glm.hpp>
#include <limits>

namespace R3 {

/**
 * @brief Explicit undefined type for integral values
 *
 * For integral types undefined resolves to UINT**_MAX or -1
 * For floating point types undefined resolves to NaN
 */
struct undefined_t {
    /**
     * @brief Cast to desired integral type
     * @tparam T
     */
    template <std::integral T>
    consteval operator T() const {
        return ~T{};
    }

    /**
     * @brief Cast to desired float type
     * @tparam T
     */
    template <std::floating_point T>
    consteval operator T() const {
        return std::numeric_limits<T>::quiet_NaN();
    }
};

static constexpr undefined_t undefined; /**< undefined constant */

/**
 * @brief Compare equality of undefined and numeric type
 * @param x Var to check equality
 * @return Does x equal undefined?
 */
/** @{ */
constexpr bool operator==(undefined_t, std::integral auto x) {
    return x == ~decltype(x){};
}

constexpr bool operator!=(undefined_t, std::integral auto x) {
    return x != ~decltype(x){};
}

constexpr bool operator==(std::integral auto x, undefined_t) {
    return x == ~decltype(x){};
}

constexpr bool operator!=(std::integral auto x, undefined_t) {
    return x != ~decltype(x){};
}

constexpr bool operator==(undefined_t, std::floating_point auto x) {
    return std::isnan(x);
}

constexpr bool operator!=(undefined_t, std::floating_point auto x) {
    return !std::isnan(x);
}

constexpr bool operator==(std::floating_point auto x, undefined_t) {
    return std::isnan(x);
}

constexpr bool operator!=(std::floating_point auto x, undefined_t) {
    return !std::isnan(x);
}
/** @} */

using glm::vec1;
using glm::vec2;
using glm::vec3;
using glm::vec4;

using glm::ivec1;
using glm::ivec2;
using glm::ivec3;
using glm::ivec4;

using glm::uvec1;
using glm::uvec2;
using glm::uvec3;
using glm::uvec4;

using glm::fvec1;
using glm::fvec2;
using glm::fvec3;
using glm::fvec4;

using glm::dvec1;
using glm::dvec2;
using glm::dvec3;
using glm::dvec4;

using glm::mat2;
using glm::mat3;
using glm::mat4;

using glm::fmat2;
using glm::fmat3;
using glm::fmat4;

using glm::dmat2;
using glm::dmat3;
using glm::dmat4;

using glm::quat;

/**
 * @brief R3 primitive type
 */
/** @{ */
using int8  = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;
#if SIZE_MAX == UINT_MAX
using isize = int;
#elif SIZE_MAX == ULONG_MAX
using isize = long;
#else
using isize = long long;
#endif

using uint8  = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;
using usize  = std::size_t;

using uuid32 = uint32;
using uuid64 = uint64;
/** @} */

} // namespace R3
