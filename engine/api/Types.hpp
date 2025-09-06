#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "engine/api/Api.hpp"

namespace R3 {

using flag_t = uint32_t; /// bitmask flag type

using uint8  = std::uint8_t;       /// 8-bit unsigned integer
using uint16 = std::uint16_t;      /// 16-bit unsigned integer
using uint32 = std::uint32_t;      /// 32-bit unsigned integer
using uint64 = std::uint64_t;      /// 64-bit unsigned integer
using usize  = std::size_t;        /// unsigned platform size type
using usize2 = glm::vec<2, usize>; /// unsigned platform size 2 dimensional vector
using usize3 = glm::vec<3, usize>; /// unsigned platform size 3 dimensional vector

using int8  = std::int8_t;  /// 8-bit signed integer
using int16 = std::int16_t; /// 16-bit signed integer
using int32 = std::int32_t; /// 32-bit signed integer
using int64 = std::int64_t; /// 64-bit signed integer

using glm::uvec2; /// unsigned integer 2 dimensional vector
using glm::uvec3; /// unsigned integer 3 dimensional vector
using glm::uvec4; /// unsigned integer 4 dimensional vector

using glm::u8vec2; /// unsigned 8-bit integer 2 dimensional vector
using glm::u8vec3; /// unsigned 8-bit integer 3 dimensional vector
using glm::u8vec4; /// unsigned 8-bit integer 4 dimensional vector

using glm::u16vec2; /// unsigned 16-bit integer 2 dimensional vector
using glm::u16vec3; /// unsigned 16-bit integer 3 dimensional vector
using glm::u16vec4; /// unsigned 16-bit integer 4 dimensional vector

using glm::u32vec2; /// unsigned 32-bit integer 2 dimensional vector
using glm::u32vec3; /// unsigned 32-bit integer 3 dimensional vector
using glm::u32vec4; /// unsigned 32-bit integer 4 dimensional vector

using glm::u64vec2; /// unsigned 64-bit integer 2 dimensional vector
using glm::u64vec3; /// unsigned 64-bit integer 3 dimensional vector
using glm::u64vec4; /// unsigned 64-bit integer 4 dimensional vector

using glm::ivec2; /// signed integer 2 dimensional vector
using glm::ivec3; /// signed integer 3 dimensional vector
using glm::ivec4; /// signed integer 4 dimensional vector

using glm::i8vec2; /// signed 8-bit integer 2 dimensional vector
using glm::i8vec3; /// signed 8-bit integer 3 dimensional vector
using glm::i8vec4; /// signed 8-bit integer 4 dimensional vector

using glm::i16vec2; /// signed 16-bit integer 2 dimensional vector
using glm::i16vec3; /// signed 16-bit integer 3 dimensional vector
using glm::i16vec4; /// signed 16-bit integer 4 dimensional vector

using glm::i32vec2; /// signed 32-bit integer 2 dimensional vector
using glm::i32vec3; /// signed 32-bit integer 3 dimensional vector
using glm::i32vec4; /// signed 32-bit integer 4 dimensional vector

using glm::i64vec2; /// signed 64-bit integer 2 dimensional vector
using glm::i64vec3; /// signed 64-bit integer 3 dimensional vector
using glm::i64vec4; /// signed 64-bit integer 4 dimensional vector

using glm::fvec2; /// floating point 2 dimensional vector
using glm::fvec3; /// floating point 3 dimensional vector
using glm::fvec4; /// floating point 4 dimensional vector

using glm::dvec2; /// floating point 2 dimensional vector
using glm::dvec3; /// floating point 3 dimensional vector
using glm::dvec4; /// floating point 4 dimensional vector

using glm::fmat2; /// floating point 2x2 dimensional matrix
using glm::fmat3; /// floating point 3x3 dimensional matrix
using glm::fmat4; /// floating point 4x4 dimensional matrix

using glm::dmat2; /// floating point 2x2 dimensional matrix
using glm::dmat3; /// floating point 3x3 dimensional matrix
using glm::dmat4; /// floating point 4x4 dimensional matrix

using glm::fquat; /// 32-bit floating point quaternion

using glm::dquat; /// 32-bit floating point quaternion

} // namespace R3
