#pragma once
#include "Shader.hpp"
#include "../IRenderer.hpp"

namespace R3::opengl {

class Renderer : protected IRenderer {
public:
  Renderer();
  ~Renderer();

  void draw() override;

private:
  Shader _shader; // TODO(jon): make it the Actors responsibility
  unsigned _texture;
  unsigned _vao, _vbo, _ebo;
};

} // namespace R3::opengl
