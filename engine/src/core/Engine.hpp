#pragma once
#include "Renderer.hpp"
#include "Window.hpp"
#include <functional>

namespace R3 {

class Engine final {
private:
  Engine();

public:
  Engine(const Engine&) = delete;
  void operator=(const Engine&) = delete;

  static Engine* instance();
  int run();

public:
  Window window;

private:
  Renderer _renderer;
};

} // namespace R3
