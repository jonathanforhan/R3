#pragma once
#if R3_OPENGL && (R3_VULKAN || R3_DX11 || R3_DX12)
#elif R3_VULKAN && (R3_DX11 || R3_DX12 || R3_OPENGL)
#elif R3_DX11 && (R3_DX12 || R3_OPENGL || R3_VULKAN)
#elif R3_DX12 && (R3_OPENGL || R3_VULKAN || R3_DX11)
#error "Multiple Renderers"
#endif

#include <vector>
#include "core/Actor.hpp"
#include "core/Renderer.hpp"
#include "core/Window.hpp"

namespace R3 {

class Engine {
private:
  Engine();

public:
  Engine(const Engine&) = delete;
  void operator=(const Engine&) = delete;

  static void initialize() { (void)instance(); }
  static Engine* const instance();
  bool running() const { return !window.should_close(); }
  void update();

  void add_actor(Actor* actor);

public:
  Window window;

private:
  Renderer _renderer;
  std::vector<Actor*> _actors;
};

} // namespace R3
