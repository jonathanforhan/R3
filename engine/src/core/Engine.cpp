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
    if (action == InputAction::Press) {
      mouse_down = true;
    } else if (action == InputAction::Release) {
      mouse_down = false;
    }
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

  curr.x = _input.cursor_position().x;
  curr.y = _input.cursor_position().y;
  if (mouse_down) {
    float dx = curr.x - prev.x;
    float dy = prev.y - curr.y;
    float s = 500;
    _camera.look_around(dx * s, dy * s);
  }
  prev = curr;
  static mat4 view = mat4(1.0f), projection = mat4(1.0f);
  _camera.apply(&view, &projection, _window.aspect_ratio());

  _renderer.predraw();

  for (auto entity : _entities) {
    entity->tick(delta_time);

    entity->_texture.bind(0);
    entity->_shader.use();

    entity->write_to_shader(MODEL_LOCATION, entity->model);
    entity->write_to_shader(VIEW_LOCATION, view);
    entity->write_to_shader(PROJECTION_LOCATION, projection);

    entity->_mesh.bind();

    _renderer.draw(RendererPrimitive::Triangles, entity->_mesh.number_of_indices());
  }
  _window.update();
  _input.poll_keys();
  _input.poll_mouse();
}

void Engine::add_entity(Entity* entity) {
  entity->_texture.bind(0);
  entity->_shader.use();
  entity->write_to_shader(entity->_texture.name(), 0);
  _entities.push_back(entity);
}

} // namespace R3
