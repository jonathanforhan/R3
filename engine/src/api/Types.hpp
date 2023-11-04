#pragma once
#include <cstdint>
#include <glm/glm.hpp>

namespace R3 {

// vecs are first class citizens
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

// mats are first class citizens
using glm::mat2;
using glm::mat3;
using glm::mat4;

using glm::fmat2;
using glm::fmat3;
using glm::fmat4;

using glm::dmat2;
using glm::dmat3;
using glm::dmat4;

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
#if SIZE_MAX == UINT_MAX
using isize = int;
#elif SIZE_MAX == ULONG_MAX
using isize = long;
#elif SIZE_MAX == ULLONG_MAX
using isize = long long;
#endif

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;
using usize = size_t;

} // namespace R3
