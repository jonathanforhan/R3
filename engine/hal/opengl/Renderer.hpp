#pragma once
#include "hal/IRenderer.hpp"
#include "hal/opengl/Shader.hpp"
#include "hal/opengl/Texture.hpp"

namespace R3::opengl {

class Window;

class Renderer : protected IRenderer {
public:
  explicit Renderer(Window *window);
  ~Renderer();

  void draw() override;

private:
  Window* _window;
  Shader _shader;   // TODO
  Texture _texture; // TODO
  uint32_t _vao, _vbo, _ebo;
};

} // namespace R3::opengl
