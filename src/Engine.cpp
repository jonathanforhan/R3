#include "Engine.hpp"

namespace R3 {

Engine::Engine()
    : _window(Window(800, 600, "R3")),
      _renderer() {}

int Engine::run() {
  while (!_window.should_close()) {
    _renderer.draw();
    _window.update();
    glfwPollEvents();
  }
  return 0;
}

} // namespace R3
