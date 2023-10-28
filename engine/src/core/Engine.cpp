#include "Engine.hpp"
#include <api/Log.hpp>
#include <api/Math.hpp>
#include <api/Clock.hpp>

constexpr uint32_t MODEL_LOCATION = 0;
constexpr uint32_t VIEW_LOCATION = 1;
constexpr uint32_t PROJECTION_LOCATION = 2;

namespace R3 {

Engine::Engine()
    : window(Window("R3")),
      _renderer(&window) {
  window.show();
}

Engine* const Engine::instance() {
  static Engine _instance;
  return &_instance;
}

void Engine::update() {
  mat4 view = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, -3, 1}};
  mat4 projection = glm::perspective(glm::radians(45.0f), window.aspect_ratio(), 0.1f, 100.0f);

  static double then = Clock::current_time();
  double now = Clock::current_time();
  double delta_time = now - then;
  then = now;

  _renderer.predraw();

  for (auto* actor : _actors) {
    actor->tick(delta_time);

    actor->_texture.bind(0);
    actor->_shader.use();

    actor->write_to_shader(MODEL_LOCATION, actor->model);
    actor->write_to_shader(VIEW_LOCATION, view);
    actor->write_to_shader(PROJECTION_LOCATION, projection);

    actor->_mesh.bind();

    _renderer.draw(RendererPrimitive::Triangles, actor->_mesh.number_of_indices());
  }
  window.update();
}

void Engine::add_actor(Actor* actor) {
  actor->_texture.bind(0);
  actor->_shader.use();
  actor->write_to_shader(actor->_texture.name(), 0);
  _actors.push_back(actor);
}

} // namespace R3
