#include "Engine.hpp"

namespace R3 {

Engine::Engine()
    : _window(Window("R3")),
      _renderer(&_window) {}

int Engine::run() {
  _window.show();

  while (!_window.should_close()) {
    _renderer.draw();
    _window.update();
  }
  return 0;
}

} // namespace R3
