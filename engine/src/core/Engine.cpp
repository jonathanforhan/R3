#include "Engine.hpp"
#include <api/Check.hpp>
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
    _entities.reserve(1000000);

    _input.set_key_binding(Key::Key_W, [this](InputAction) { _camera.translate_forward(0.1f); });
    _input.set_key_binding(Key::Key_A, [this](InputAction) { _camera.translate_left(0.08f); });
    _input.set_key_binding(Key::Key_S, [this](InputAction) { _camera.translate_backward(0.1f); });
    _input.set_key_binding(Key::Key_D, [this](InputAction) { _camera.translate_right(0.08f); });
    _input.set_key_binding(Key::Key_Q, [this](InputAction) { _camera.translate_up(0.1f); });
    _input.set_key_binding(Key::Key_E, [this](InputAction) { _camera.translate_down(0.1f); });

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

void Engine::loop() {
    while (!_window.should_close()) {
        update();
    }
}

void Engine::update() {
    static double then = Clock::current_time();
    double now = Clock::current_time();
    double delta_time = now - then;
    then = now;

    curr.x = (float)_input.cursor_position().x;
    curr.y = (float)_input.cursor_position().y;
    if (mouse_down) {
        float dx = curr.x - prev.x;
        float dy = prev.y - curr.y;
        float s = 500;
        _camera.look_around(dx * s, dy * s);
    }
    prev = curr;

    _camera.apply(&view, &projection, _window.aspect_ratio());

    _renderer.predraw();

    for (auto& entity : _entities) {
        for (auto& component : entity->components()) {
            component->parent = entity.get();
            component->tick(delta_time);
        }
        entity->tick(delta_time);
    }
    _window.update();
    _input.poll_keys();
    _input.poll_mouse();
}

void Engine::draw_indexed(RendererPrimitive primitive, uint32 n_indices) {
    _renderer.draw(primitive, n_indices);
}

void Engine::add_entity(Entity* entity) {
    _entities.emplace_back(entity);
}

void Engine::register_component(const std::string& name, Component* component) {
    CHECK(!_components.contains(name));
    _components.emplace(name, component);
}

Component* Engine::component_ptr(const std::string& name) const {
    CHECK(_components.contains(name));
    return _components.contains(name) ? _components.at(name).get() : nullptr;
}

void Engine::register_mesh(const std::string& name, Mesh mesh) {
    CHECK(!_meshes.contains(name));
    _meshes[name] = mesh;
}

uint32 Engine::mesh_id(const std::string& name) const {
    CHECK(_meshes.contains(name));
    return _meshes.contains(name) ? _meshes.at(name).id() : 0;
}

void Engine::register_shader(const std::string& name, Shader shader) {
    CHECK(!_shaders.contains(name));
    _shaders[name] = shader;
}

uint32 Engine::shader_id(const std::string& name) const {
    CHECK(_shaders.contains(name));
    return _shaders.contains(name) ? _shaders.at(name).id() : 0;
}

void Engine::register_texture2D(const std::string& name, Texture2D texture) {
    CHECK(!_textures.contains(name));
    _textures[name] = texture;
}

uint32 Engine::texture2D_id(const std::string& name) const {
    CHECK(_textures.contains(name));
    return _textures.contains(name) ? _textures.at(name).id() : 0;
}

} // namespace R3
