#pragma once
#include <cstdint>

namespace R3 {

enum class ShaderType { GLSL, HLSL, SPIRV };

class Shader {
public:
  Shader() = default;
  Shader(ShaderType type, const char* vert, const char* frag);

  void assign(Shader shader);
  void destroy();
  void use();
  bool valid() const { return _id >= 0; }

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
  void import_glsl(const char* vert, const char* frag);

private:
  int64_t _id = -1;
};

} // namespace R3

#ifdef R3_OPENGL
#include "hal/opengl/Shader.ipp"
#endif
