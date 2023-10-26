#pragma once
#include <glad/glad.h>
#include <cstdint>

namespace R3::opengl {

class Shader {
private:
  Shader() = default;

public:
  ~Shader();

  static Shader from_source(const char* vert, const char* frag);
  // static Shader from_spriv(const char*, const char*); TODO

  void use();

  uint32_t location(const char* uniform) const;

  void write_uniform(uint32_t location, float v0);
  void write_uniform(uint32_t location, int v0);
  void write_uniform(uint32_t location, unsigned v0);
  template <typename T>
  void write_uniform(uint32_t location, T v0, T v1);
  template <typename T>
  void write_uniform(uint32_t location, T v0, T v1, T v2);
  template <typename T>
  void write_uniform(uint32_t location, T v0, T v1, T v2, T v3);
  template <typename T>
  void write_uniform(uint32_t location, const T& v0);

private:
  uint32_t _id;
};

} // namespace R3::opengl

#include "Shader.ipp"
