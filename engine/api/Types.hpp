#pragma once

#include <cstdint>
#include <glm/glm.hpp>

namespace R3 {

using uint8  = std::uint8_t;  /// 8-bit unsigned integer
using uint16 = std::uint16_t; /// 16-bit unsigned integer
using uint32 = std::uint32_t; /// 32-bit unsigned integer
using uint64 = std::uint64_t; /// 64-bit unsigned integer
using usize  = std::size_t;   /// unsigned platform size type

using int8  = std::int8_t;  /// 8-bit signed integer
using int16 = std::int16_t; /// 16-bit signed integer
using int32 = std::int32_t; /// 32-bit signed integer
using int64 = std::int64_t; /// 64-bit signed integer

using glm::uvec2; /// 32-bit unsigned integer 2 dimensional vector
using glm::uvec3; /// 32-bit unsigned integer 3 dimensional vector
using glm::uvec4; /// 32-bit unsigned integer 4 dimensional vector

using glm::ivec2; /// 32-bit signed integer 2 dimensional vector
using glm::ivec3; /// 32-bit signed integer 3 dimensional vector
using glm::ivec4; /// 32-bit signed integer 4 dimensional vector

using glm::fvec2; /// 32-bit floating point 2 dimensional vector
using glm::fvec3; /// 32-bit floating point 3 dimensional vector
using glm::fvec4; /// 32-bit floating point 4 dimensional vector

using glm::dvec2; /// 64-bit floating point 2 dimensional vector
using glm::dvec3; /// 64-bit floating point 3 dimensional vector
using glm::dvec4; /// 64-bit floating point 4 dimensional vector

using glm::fmat2; /// 32-bit floating point 2x2 dimensional matrix
using glm::fmat3; /// 32-bit floating point 3x3 dimensional matrix
using glm::fmat4; /// 32-bit floating point 4x4 dimensional matrix

using glm::dmat2; /// 64-bit floating point 2x2 dimensional matrix
using glm::dmat3; /// 64-bit floating point 3x3 dimensional matrix
using glm::dmat4; /// 64-bit floating point 4x4 dimensional matrix

using glm::fquat; /// 32-bit floating point quaternion

using glm::dquat; /// 32-bit floating point quaternion

} // namespace R3