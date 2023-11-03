#include "Engine.hpp"
#include <chrono>
#include "api/Check.hpp"
#include "api/Log.hpp"
#include "api/Math.hpp"
#include "core/Scene.hpp"
#include "core/Entity.hpp"
#include "core/Camera.hpp"

constexpr uint32_t MODEL_LOCATION = 0;
constexpr uint32_t VIEW_LOCATION = 1;
constexpr uint32_t PROJECTION_LOCATION = 2;

namespace R3 {

Engine::Engine()
    : _window(Window("R3")),
      _renderer(&_window) {
}

Engine& Engine::instance() {
    static Engine _instance;
    return _instance;
}

void Engine::loop() {

    Camera _camera;

    _window.show();
    while (!_window.should_close()) {
        _renderer.predraw();

        double dt = delta_time();
        static mat4 view = mat4(1.0f), projection = mat4(1.0f);
        _camera.apply(&view, &projection, _window.aspect_ratio());

        std::ranges::for_each(_systems, [dt](const auto& fn) { fn(dt); });
        _window.update();
    }
}

void Engine::draw_indexed(RendererPrimitive primitive, uint32 n_indices) {
    _renderer.draw(primitive, n_indices);
}

double Engine::delta_time() const {
    using namespace std::chrono;
    static auto s_prev = system_clock::now();
    auto now = system_clock::now();
    double dt = duration<double>(now - s_prev).count();
    s_prev = now;
    return dt;
}

} // namespace R3
