#include "Engine.hpp"
#include <api/Check.hpp>
#include <api/Log.hpp>
#include <api/Math.hpp>

constexpr uint32_t MODEL_LOCATION = 0;
constexpr uint32_t VIEW_LOCATION = 1;
constexpr uint32_t PROJECTION_LOCATION = 2;

namespace R3 {

Engine::Engine()
    : _window(Window("R3")),
      _renderer(&_window) {
}

Engine* const Engine::instance() {
    static Engine _instance;
    return &_instance;
}

void Engine::loop() {
    _window.show();
    while (!_window.should_close()) {
        update();
    }
}

void Engine::update() {
}

void Engine::draw_indexed(RendererPrimitive primitive, uint32 n_indices) {
    _renderer.draw(primitive, n_indices);
}

} // namespace R3
