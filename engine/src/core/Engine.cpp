#include "Engine.hpp"
#include <api/Clock.hpp>
#include <api/Log.hpp>
#include <api/Math.hpp>

constexpr uint32_t MODEL_LOCATION = 0;
constexpr uint32_t VIEW_LOCATION = 1;
constexpr uint32_t PROJECTION_LOCATION = 2;

namespace R3 {

Engine::Engine()
    : _window(Window("R3")),
      _renderer(&_window),
      _input(&_window),
      _camera() {
  _window.show();

  _input.set_key_binding(Key::Key_W, [this](InputAction action) { _camera.translate_forward(0.1); });
  _input.set_key_binding(Key::Key_A, [this](InputAction action) { _camera.translate_left(0.08); });
  _input.set_key_binding(Key::Key_S, [this](InputAction action) { _camera.translate_backward(0.1); });
  _input.set_key_binding(Key::Key_D, [this](InputAction action) { _camera.translate_right(0.08); });
  _input.set_key_binding(Key::Key_Q, [this](InputAction action) { _camera.translate_up(0.1); });
  _input.set_key_binding(Key::Key_E, [this](InputAction action) { _camera.translate_down(0.1); });

  _input.set_mouse_binding(MouseButton::Left, [this](InputAction action) {
    LOG(Info, _input.cursor_position().x, _input.cursor_position().y);
  });
}

Engine* const Engine::instance() {
  static Engine _instance;
  return &_instance;
}

void Engine::update() {
  static double then = Clock::current_time();
  double now = Clock::current_time();
  double delta_time = now - then;
  then = now;

  static mat4 view = mat4(1.0f), projection = mat4(1.0f);
  _camera.apply(&view, &projection, _window.aspect_ratio());

  _renderer.predraw();

  for (auto actor : _actors) {
    actor->tick(delta_time);

    actor->_texture.bind(0);
    actor->_shader.use();

    actor->write_to_shader(MODEL_LOCATION, actor->model);
    actor->write_to_shader(VIEW_LOCATION, view);
    actor->write_to_shader(PROJECTION_LOCATION, projection);

    actor->_mesh.bind();

    _renderer.draw(RendererPrimitive::Triangles, actor->_mesh.number_of_indices());
  }
  _window.update();
  _input.poll_keys();
  _input.poll_mouse();
}

void Engine::add_actor(Actor* actor) {
  actor->_texture.bind(0);
  actor->_shader.use();
  actor->write_to_shader(actor->_texture.name(), 0);
  _actors.push_back(actor);
}

} // namespace R3
