#pragma once
#include "hal/IRenderer.hpp"

namespace R3 {
class Engine;
}

namespace R3::opengl {

class Window;

class Renderer : protected IRenderer {
private:
  explicit Renderer(Window* window);
  Renderer(const Renderer&) = delete;
  void operator=(const Renderer&) = delete;
  friend class R3::Engine;

public:
  ~Renderer();

  void predraw() override;
  void draw(RendererPrimitive primitive, uint32 n_indices) override;

private:
  Window* _window;
};

} // namespace R3::opengl
