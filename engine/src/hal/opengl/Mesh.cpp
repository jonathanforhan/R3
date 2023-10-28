#if R3_OPENGL

#include "core/Mesh.hpp"
#include <numeric>
#include <glad/glad.h>
#include "api/Log.hpp"

namespace R3 {

Mesh::Mesh(std::span<float> vertices, std::span<uint32_t> stride, std::span<uint32_t> indices) {
  write_buffer(vertices, stride, indices);
}

void Mesh::assign(Mesh mesh) {
  if (mesh._vao == _vao) {
    return;
  }
  destroy();
  _vao = mesh._vao;
  _vbo = mesh._vbo;
  _ebo = mesh._ebo;
  _number_of_indices = mesh._number_of_indices;
}

void Mesh::destroy() {
  if (_vao >= 0)
    glDeleteVertexArrays(1, reinterpret_cast<GLuint*>(&_vao));
  if (_vbo >= 0)
    glDeleteBuffers(1, reinterpret_cast<GLuint*>(&_vbo));
  if (_ebo >= 0)
    glDeleteBuffers(1, reinterpret_cast<GLuint*>(&_ebo));

  _vao = _vbo = _ebo = -1;
}

void Mesh::write_buffer(std::span<float> vertices, std::span<uint32_t> stride, std::span<uint32_t> indices) {
  destroy();
  _number_of_indices = indices.size();

  GLuint vao, buffers[2];
  glGenVertexArrays(1, &vao);
  glGenBuffers(2, buffers);
  _vao = vao, _vbo = buffers[0], _ebo = buffers[1];

  glBindVertexArray(_vao);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size_bytes(), vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size_bytes(), indices.data(), GL_STATIC_DRAW);

  uint32_t byte_span = std::reduce(stride.begin(), stride.end()) * sizeof(float);
  uint8_t* byte_index = 0;

  for (uint32_t i = 0; i < stride.size(); i++) {
    glVertexAttribPointer(i, stride[i], GL_FLOAT, GL_FALSE, byte_span, byte_index);
    glEnableVertexAttribArray(i);
    byte_index += stride[i] * sizeof(float);
  }

  glBindBuffer(GL_ARRAY_BUFFER, NULL);
  glBindVertexArray(NULL);
}

void Mesh::bind() const {
  glBindVertexArray(_vao);
}

void Mesh::bind_null() const {
  glBindVertexArray(NULL);
}

} // namespace R3

#endif // R3_OPENGL
