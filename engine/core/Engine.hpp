#pragma once
#include "Renderer.hpp"
#include "Window.hpp"

namespace R3 {

class Engine final {
private:
  Engine();

public:
  Engine(const Engine&) = delete;
  void operator=(const Engine&) = delete;

  static Engine& instance();
  int run();

private:
  Window _window;
  Renderer _renderer;
};

inline Engine& Engine::instance() {
  static Engine _instance;
  return _instance;
}

} // namespace R3
