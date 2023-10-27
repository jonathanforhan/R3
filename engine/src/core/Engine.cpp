#include "Engine.hpp"
#include <api/Log.hpp>

namespace R3 {

Engine::Engine()
    : window(Window("R3")),
      _renderer(&window) {}

Engine* Engine::instance() {
  static Engine _instance;
  return &_instance;
}

int Engine::run() {
  window.show();

  while (!window.should_close()) {
    _renderer.draw();
    window.update();
  }
  return 0;
}

} // namespace R3
