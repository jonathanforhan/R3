#pragma once
#include <cstdint>

namespace R3::opengl {

class Shader {
private:
  Shader() = default;

public:
  ~Shader();

  static Shader from_source(const char* vert, const char* frag);
  static Shader from_spriv(const char*, const char*) { throw; /* TODO */ }

  void use();

private:
  uint32_t _program;
};

} // namespace R3::opengl
