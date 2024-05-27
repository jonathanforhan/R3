/*******************************************************************************
 * @file Types.hpp
 * @brief Defines R3 types
 ******************************************************************************/

#pragma once

#include <concepts>
#include <cstdint>
#include <glm/glm.hpp>

namespace R3 {

namespace detail {

/*******************************************************************************
 * @brief Explicit undefined type for integral values
 *
 * Correct regardless of type and sign, resovles to UINT**_MAX or -1
 ******************************************************************************/
struct undefined_t {
    /***************************************************************************
     * @brief Cast to desired integral type
     * @tparam T
     **************************************************************************/
    template <std::integral T>
    consteval operator T() const {
        return ~T{};
    }
};

} // namespace detail

static thread_local constexpr detail::undefined_t undefined; /**< thread undefined constant */

/*******************************************************************************
 * @brief Compare equality of undefined and integral type
 * @param x Var to check equality
 * @return Does x equal undefined?
 ******************************************************************************/
/** @{ */
constexpr bool operator==(detail::undefined_t, std::integral auto x) {
    return x == ~decltype(x){};
}

constexpr bool operator!=(detail::undefined_t, std::integral auto x) {
    return x != ~decltype(x){};
}

constexpr bool operator==(std::integral auto x, detail::undefined_t) {
    return x == ~decltype(x){};
}

constexpr bool operator!=(std::integral auto x, detail::undefined_t) {
    return x != ~decltype(x){};
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

/*******************************************************************************
 * @brief R3 primitive type
 ******************************************************************************/
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
