#pragma once
#include <cstdint>
#include <span>

namespace R3 {

/* Implemented in respective HAL dir */
class Mesh {
public:
  Mesh() = default;
  Mesh(std::span<float> vertices, std::span<uint32_t> stride, std::span<uint32_t> indices);

  void assign(Mesh mesh);
  void destroy();
  bool valid() const { return _vao >= 0 && _vbo >= 0 && _ebo >= 0; }
  void write_buffer(std::span<float> vertices, std::span<uint32_t> stride, std::span<uint32_t> indices);
  void bind() const;
  void bind_null() const;
  size_t number_of_indices() const { return _number_of_indices; }

private:
  int64_t _vao = -1;
  int64_t _vbo = -1;
  int64_t _ebo = -1;
  size_t _number_of_indices = -1;
};

} // namespace R3
