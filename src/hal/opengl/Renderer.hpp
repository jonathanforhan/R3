#pragma once
#include "Shader.hpp"
#include "hal/IRenderer.hpp"

namespace R3::opengl {

class Renderer : protected IRenderer {
public:
  Renderer();
  ~Renderer();

  void draw() override;

private:
  Shader _shader; // TODO
  unsigned _vao, _vbo, _ebo;
};

} // namespace R3::opengl
