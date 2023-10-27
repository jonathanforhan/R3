#pragma once
#include <cstdint>

namespace R3::opengl {

class Texture {
public:
  explicit Texture(const char* path);

  void bind(uint8_t index) const;

private:
  uint32_t _id;
};

} // namespace R3::opengl
