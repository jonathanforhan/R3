#if R3_OPENGL

#pragma once
#include "core/Shader.hpp"
#include <glad/glad.h>
#include "api/Math.hpp"

namespace R3 {

inline void Shader::write_uniform(uint32_t location, GLfloat v0) {
  glUniform1f(location, v0);
}

template <>
inline void Shader::write_uniform(uint32_t location, GLfloat v0, GLfloat v1) {
  glUniform2f(location, v0, v1);
}

template <>
inline void Shader::write_uniform(uint32_t location, GLfloat v0, GLfloat v1, GLfloat v2) {
  glUniform3f(location, v0, v1, v2);
}

template <>
inline void Shader::write_uniform(uint32_t location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) {
  glUniform4f(location, v0, v1, v2, v3);
}

inline void Shader::write_uniform(uint32_t location, GLint v0) {
  glUniform1i(location, v0);
}

template <>
inline void Shader::write_uniform(uint32_t location, GLint v0, GLint v1) {
  glUniform2i(location, v0, v1);
}

template <>
inline void Shader::write_uniform(uint32_t location, GLint v0, GLint v1, GLint v2) {
  glUniform3i(location, v0, v1, v2);
}

template <>
inline void Shader::write_uniform(uint32_t location, GLint v0, GLint v1, GLint v2, GLint v3) {
  glUniform4i(location, v0, v1, v2, v3);
}

inline void Shader::write_uniform(uint32_t location, GLuint v0) {
  glUniform1ui(location, v0);
}

template <>
inline void Shader::write_uniform(uint32_t location, GLuint v0, GLuint v1) {
  glUniform2ui(location, v0, v1);
}

template <>
inline void Shader::write_uniform(uint32_t location, GLuint v0, GLuint v1, GLuint v2) {
  glUniform3ui(location, v0, v1, v2);
}

template <>
inline void Shader::write_uniform(uint32_t location, GLuint v0, GLuint v1, GLuint v2, GLuint v3) {
  glUniform4ui(location, v0, v1, v2, v3);
}

template <>
inline void Shader::write_uniform(uint32_t location, const glm::vec1& v0) {
  glUniform1fv(location, 1, glm::value_ptr(v0));
}

template <>
inline void Shader::write_uniform(uint32_t location, const glm::vec2& v0) {
  glUniform2fv(location, 1, glm::value_ptr(v0));
}

template <>
inline void Shader::write_uniform(uint32_t location, const glm::vec3& v0) {
  glUniform3fv(location, 1, glm::value_ptr(v0));
}

template <>
inline void Shader::write_uniform(uint32_t location, const glm::vec4& v0) {
  glUniform4fv(location, 1, glm::value_ptr(v0));
}

template <>
inline void Shader::write_uniform(uint32_t location, const glm::ivec1& v0) {
  glUniform1iv(location, 1, glm::value_ptr(v0));
}

template <>
inline void Shader::write_uniform(uint32_t location, const glm::ivec2& v0) {
  glUniform2iv(location, 1, glm::value_ptr(v0));
}

template <>
inline void Shader::write_uniform(uint32_t location, const glm::ivec3& v0) {
  glUniform3iv(location, 1, glm::value_ptr(v0));
}

template <>
inline void Shader::write_uniform(uint32_t location, const glm::ivec4& v0) {
  glUniform4iv(location, 1, glm::value_ptr(v0));
}

template <>
inline void Shader::write_uniform(uint32_t location, const glm::uvec1& v0) {
  glUniform1uiv(location, 1, glm::value_ptr(v0));
}

template <>
inline void Shader::write_uniform(uint32_t location, const glm::uvec2& v0) {
  glUniform2uiv(location, 1, glm::value_ptr(v0));
}

template <>
inline void Shader::write_uniform(uint32_t location, const glm::uvec3& v0) {
  glUniform3uiv(location, 1, glm::value_ptr(v0));
}

template <>
inline void Shader::write_uniform(uint32_t location, const glm::uvec4& v0) {
  glUniform4uiv(location, 1, glm::value_ptr(v0));
}

template <>
inline void Shader::write_uniform(uint32_t location, const glm::mat2& v0) {
  glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(v0));
}

template <>
inline void Shader::write_uniform(uint32_t location, const glm::mat3& v0) {
  glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(v0));
}

template <>
inline void Shader::write_uniform(uint32_t location, const glm::mat4& v0) {
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(v0));
}

template <>
inline void Shader::write_uniform(uint32_t location, const glm::mat2x3& v0) {
  glUniformMatrix2x3fv(location, 1, GL_FALSE, glm::value_ptr(v0));
}

template <>
inline void Shader::write_uniform(uint32_t location, const glm::mat3x2& v0) {
  glUniformMatrix3x2fv(location, 1, GL_FALSE, glm::value_ptr(v0));
}

template <>
inline void Shader::write_uniform(uint32_t location, const glm::mat2x4& v0) {
  glUniformMatrix2x4fv(location, 1, GL_FALSE, glm::value_ptr(v0));
}

template <>
inline void Shader::write_uniform(uint32_t location, const glm::mat4x2& v0) {
  glUniformMatrix4x2fv(location, 1, GL_FALSE, glm::value_ptr(v0));
}

template <>
inline void Shader::write_uniform(uint32_t location, const glm::mat3x4& v0) {
  glUniformMatrix3x4fv(location, 1, GL_FALSE, glm::value_ptr(v0));
}

template <>
inline void Shader::write_uniform(uint32_t location, const glm::mat4x3& v0) {
  glUniformMatrix4x3fv(location, 1, GL_FALSE, glm::value_ptr(v0));
}

} // namespace R3

#endif // R3_OPENGL
