#pragma once

/// @file Types.hpp
/// @brief Provides basic type for Engine

#include <cstdint>
#include <glm/glm.hpp>
#include "api/Api.hpp"

namespace R3 {

// vecs are first class citizens
using glm::vec1; ///< vec1
using glm::vec2; ///< vec2
using glm::vec3; ///< vec3
using glm::vec4; ///< vec4

using glm::ivec1; ///< ivec1
using glm::ivec2; ///< ivec2
using glm::ivec3; ///< ivec3
using glm::ivec4; ///< ivec4

using glm::uvec1; ///< uvec1
using glm::uvec2; ///< uvec2
using glm::uvec3; ///< uvec3
using glm::uvec4; ///< uvec4

using glm::fvec1; ///< fvec1
using glm::fvec2; ///< fvec2
using glm::fvec3; ///< fvec3
using glm::fvec4; ///< fvec4

using glm::dvec1; ///< dvec1
using glm::dvec2; ///< dvec2
using glm::dvec3; ///< dvec3
using glm::dvec4; ///< dvec4

// mats are first class citizens
using glm::mat2; ///< mat2
using glm::mat3; ///< mat3
using glm::mat4; ///< mat4

using glm::fmat2; ///< fmat2
using glm::fmat3; ///< fmat3
using glm::fmat4; ///< fmat4

using glm::dmat2; ///< dmat2
using glm::dmat3; ///< dmat3
using glm::dmat4; ///< dmat4

using glm::quat; ///< quat

using int8 = std::int8_t;   ///< int8
using int16 = std::int16_t; ///< int16
using int32 = std::int32_t; ///< int32
using int64 = std::int64_t; ///< int64
#if SIZE_MAX == UINT_MAX
using isize = int; ///< isize
#elif SIZE_MAX == ULONG_MAX
using isize = long; ///< isize
#else
using isize = long long; ///< isize
#endif

using uint8 = std::uint8_t;   ///< uint8
using uint16 = std::uint16_t; ///< uint16
using uint32 = std::uint32_t; ///< uint32
using uint64 = std::uint64_t; ///< uint64
using usize = std::size_t;    ///< usize

using uuid32 = uint32; ///< uuid32
using uuid64 = uint64; ///< uuid64

using id = usize;

} // namespace R3

// vec export
template class R3_API glm::vec<1, glm::f32, glm::packed_highp>;
template class R3_API glm::vec<2, glm::f32, glm::packed_highp>;
template class R3_API glm::vec<3, glm::f32, glm::packed_highp>;
template class R3_API glm::vec<4, glm::f32, glm::packed_highp>;
template class R3_API glm::vec<1, glm::f64, glm::packed_highp>;
template class R3_API glm::vec<2, glm::f64, glm::packed_highp>;
template class R3_API glm::vec<3, glm::f64, glm::packed_highp>;
template class R3_API glm::vec<4, glm::f64, glm::packed_highp>;
template class R3_API glm::vec<1, glm::i32, glm::packed_highp>;
template class R3_API glm::vec<2, glm::i32, glm::packed_highp>;
template class R3_API glm::vec<3, glm::i32, glm::packed_highp>;
template class R3_API glm::vec<4, glm::i32, glm::packed_highp>;
template class R3_API glm::vec<1, glm::i64, glm::packed_highp>;
template class R3_API glm::vec<2, glm::i64, glm::packed_highp>;
template class R3_API glm::vec<3, glm::i64, glm::packed_highp>;
template class R3_API glm::vec<4, glm::i64, glm::packed_highp>;
template class R3_API glm::vec<1, glm::u32, glm::packed_highp>;
template class R3_API glm::vec<2, glm::u32, glm::packed_highp>;
template class R3_API glm::vec<3, glm::u32, glm::packed_highp>;
template class R3_API glm::vec<4, glm::u32, glm::packed_highp>;
template class R3_API glm::vec<1, glm::u64, glm::packed_highp>;
template class R3_API glm::vec<2, glm::u64, glm::packed_highp>;
template class R3_API glm::vec<3, glm::u64, glm::packed_highp>;
template class R3_API glm::vec<4, glm::u64, glm::packed_highp>;

// mat export
template class R3_API glm::mat<2, 2, glm::f32, glm::packed_highp>;
template class R3_API glm::mat<3, 3, glm::f32, glm::packed_highp>;
template class R3_API glm::mat<4, 4, glm::f32, glm::packed_highp>;
template class R3_API glm::mat<2, 2, glm::f64, glm::packed_highp>;
template class R3_API glm::mat<3, 3, glm::f64, glm::packed_highp>;
template class R3_API glm::mat<4, 4, glm::f64, glm::packed_highp>;

// quat export
template class R3_API glm::qua<glm::f64, glm::packed_highp>;
