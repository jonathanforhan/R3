#if R3_OPENGL

#include "core/Mesh.hpp"
#include <glad/glad.h>

namespace R3 {

Mesh::Mesh(std::span<Vertex> vertices, std::span<uint32> indices) {
  glGenVertexArrays(1, &_vao);
  glGenBuffers(1, &_vbo);
  glGenBuffers(1, &_ebo);

  glBindVertexArray(_vao);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size_bytes(), vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size_bytes(), indices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_coords));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, NULL);
  glBindVertexArray(NULL);
}

void Mesh::destroy() {
  glDeleteVertexArrays(1, &_vao);
  glDeleteBuffers(1, &_vbo);
  glDeleteBuffers(1, &_ebo);
}

void Mesh::bind() const {
  glBindVertexArray(_vao);
}

}// namespace R3

#endif // R3_OPENGL